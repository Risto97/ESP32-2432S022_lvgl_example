#include "esp_log.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <lvgl.h>
#include <demos/lv_demos.h>
#include <map>
#include "freertos/projdefs.h"
#include "smart_fridge.h"
#include "src/core/lv_obj_tree.h"
#include "wifi_utils.h"
#include "http_utils.h"
#include "nvs_flash.h"

#include "gui_config.h"
#include "app.h"

TaskHandle_t gui_task_handle = nullptr;
QueueHandle_t queue;

extern std::map<uint32_t, Location> locs_map;
extern std::map<uint32_t, Product> prods_map;

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
    }

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

    wifi_init_sta();

	// xTaskCreatePinnedToCore(wifi_init_sta, "wifi_init_sta", 4096*2, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(&http_get_task, "http_get_task", 4096, NULL, 5, NULL, 1);
	xTaskCreatePinnedToCore(gui_task, "gui", 4096*2, NULL, 0, &gui_task_handle, 0);
    // gui_task();
    // queue = xQueueCreate(5, 10);



    while(1){
        vTaskDelay(1000);
    }

}
