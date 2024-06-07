#include "esp_log.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <lvgl.h>
#include <demos/lv_demos.h>
#include <map>
#include "hal/uart_types.h"
#include "smart_fridge.h"
#include "src/core/lv_obj.h"
#include "src/core/lv_obj_tree.h"
#include "src/widgets/lv_label.h"
#include "src/widgets/lv_textarea.h"
#include "wifi_utils.h"
#include "http_utils.h"
#include "nvs_flash.h"
#include "driver/uart.h"

#include "gui_config.h"
#include "app.h"

TaskHandle_t gui_task_handle = nullptr;
QueueHandle_t queue;
QueueHandle_t grocy_cmd_queue;

Grocy *g_grocy;

extern std::map<uint32_t, Location> locs_map;
extern std::map<uint32_t, Product> prods_map;

Product *find_product_by_barcode(std::string barcode){
    for(auto &[key, value] : prods_map){
        auto barcodes = value.get_barcodes();
        for(auto str : barcodes){
            if(str == barcode){
                ESP_LOGI("BARCODES", "Found ITEM IS: [%s]", value.get_name().c_str());
                return &value;

            }
        }
    }

    ESP_LOGI("BARCODES", "No items found for barcode: %s", barcode.c_str());
    return nullptr;

}
void gui_task(void *pvParameter){
    (void) pvParameter;

    gui_init();

    lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
    lv_obj_set_size(spinner, 100, 100);
    lv_obj_center(spinner);
    // int count = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    // lv_obj_del(spinner);

    // ESP_LOGI("GUI_TASK", "Received notification %d", count);

    App app;
    char barcode_text[128];
    // lv_demo_widgets();
    bool grocy_data_fetched = false;
    while(1){
        lv_timer_handler(); /* let the GUI do its work */
		vTaskDelay(1);

        if(!grocy_data_fetched){
            int count = ulTaskNotifyTake(pdTRUE, 1);
            if(count > 0){
                grocy_data_fetched = true;
                lv_obj_del(spinner);
                app.create_app();
            }
        }
        

        if(lv_obj_is_valid(app.m_barcode_popup)){
            int len = uart_read_bytes(UART_NUM_1, barcode_text, (sizeof(barcode_text) - 1), 20 / portTICK_PERIOD_MS);
            if(len > 0){
                barcode_text[len-1] = '\0'; // TODO check if last character is \n first
                app.add_popup_barcode_text(std::string(barcode_text));
                // lv_label_set_text(app.m_barcode_textarea, barcode_text);
                Product * prod = find_product_by_barcode(std::string(barcode_text));
                if(prod != nullptr){
                    app.add_popup_scanned_product_name(prod);
                    // lv_label_set_text(app.m_scanned_product, prod->get_name().c_str());
                }
                else{
                    Product unknown;
                    app.add_popup_scanned_product_name(&unknown);
                    // lv_label_set_text(app.m_scanned_product, "Unknown product");

                }
                // lv_textarea_set_text(app.m_barcode_textarea, barcode_text);
            }
        }
    }

}

void init_barcode_scanner()
{
    /* Conbarcode_scannerfigure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 128 * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, GPIO_NUM_21, GPIO_NUM_35, GPIO_NUM_NC, GPIO_NUM_NC));
}

extern "C" void app_main(){
    //Initialize NVS
    ESP_LOGI("++++++++++++++++++++", "STARTING +++++++++++++++++++++++++++");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI("wifi station", "ESP_WIFI_MODE_STA");

    Grocy grocy(&grocy_cmd_queue);
    g_grocy = &grocy;

    wifi_init_sta();
    init_barcode_scanner();

	// xTaskCreatePinnedToCore(wifi_init_sta, "wifi_init_sta", 4096*2, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(&http_get_task, "http_get_task", 4096, NULL, 5, NULL, 1);
	xTaskCreatePinnedToCore(gui_task, "gui", 4096*2, NULL, 0, &gui_task_handle, 0);
    // gui_task();
    grocy_cmd_queue = xQueueCreate(5, sizeof(Grocy::grocy_cmd_t));



    while(1){
        vTaskDelay(1000);
    }

}
