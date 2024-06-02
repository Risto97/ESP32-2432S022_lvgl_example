#include "gui_config.h"
#include "esp_log.h"
#include "src/core/lv_obj.h"
#include <cstdint>
#include <lvgl.h>

static constexpr uint32_t LV_TICK_PERIOD_MS = 1;

static CST820 touch(TOUCH_I2C_SDA_PIN, TOUCH_I2C_SCL_PIN, GPIO_NUM_NC, GPIO_NUM_NC);
static LGFX tft;

LGFX::LGFX(void)
{
    {
        auto cfg = _bus_instance.config();
        cfg.freq_write = 25000000;
        cfg.pin_wr = 4;
        cfg.pin_rd = 2;
        cfg.pin_rs = 16;

        cfg.pin_d0 = 15;
        cfg.pin_d1 = 13;
        cfg.pin_d2 = 12;
        cfg.pin_d3 = 14;
        cfg.pin_d4 = 27;
        cfg.pin_d5 = 25;
        cfg.pin_d6 = 33;
        cfg.pin_d7 = 32;

        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);
    }

    {
        auto cfg = _panel_instance.config();

        cfg.pin_cs = 17;
        cfg.pin_rst = -1;
        cfg.pin_busy = -1;

        cfg.panel_width = 240;
        cfg.panel_height = 320;
        cfg.offset_x = 0;
        cfg.offset_y = 0;
        cfg.offset_rotation = 0;
        // cfg.dummy_read_pixel = 8;
        // cfg.dummy_read_bits = 1;
        cfg.readable = false;
        cfg.invert = false;
        cfg.rgb_order = false;
        cfg.dlen_16bit = false;
        cfg.bus_shared = true;

        _panel_instance.config(cfg);
    }

    setPanel(&_panel_instance);
}

static void disp_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    int w = (area->x2 - area->x1 + 1);
    int h = (area->y2 - area->y1 + 1);

    tft.startWrite();                            /* Start new TFT transaction */
    tft.setAddrWindow(area->x1, area->y1, w, h); /* set the working window */
    tft.writePixels(&color_p->full, w * h,false);//true

    tft.endWrite();            /* terminate TFT transaction */
    lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

static void touchscreen_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    bool touched;
    uint8_t gesture;
    uint16_t touchX, touchY;

    touched = touch.getTouch(&touchX, &touchY, &gesture);

    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        // ESP_LOGI("TOUCH", "%d, %d", touchY, touchX);
    }
}

void backlight_state(bool state){
	gpio_set_level(BACKLIGHT_PIN, state);
}

static void lv_tick_task(void *arg) {
    (void) arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

void gui_init(){
//     // Turn on backlight
    gpio_set_direction(BACKLIGHT_PIN, GPIO_MODE_OUTPUT);
    backlight_state(true);

    touch.begin();
    lv_init();
    tft.init();

    static lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(TFT_HEIGHT * 150 * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    static lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(TFT_HEIGHT * 150 * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, TFT_HEIGHT * 150);

    // initialise the display
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    // settings for display driver
    disp_drv.hor_res = TFT_WIDTH;
    disp_drv.ver_res = TFT_HEIGHT;
    disp_drv.flush_cb = disp_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);


    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchscreen_cb;
    lv_indev_drv_register(&indev_drv);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
}
