#include "gui_config.h"
#include "esp_log.h"
#include "src/core/lv_obj.h"
#include "driver/i2c.h"
#include <cstdint>
#include <lvgl.h>

static constexpr uint32_t LV_TICK_PERIOD_MS = 1;

static CST820 touch(TOUCH_I2C_SDA_PIN, TOUCH_I2C_SCL_PIN, GPIO_NUM_NC, GPIO_NUM_NC);
static LGFX tft;

LGFX::LGFX(void)
{
    {
        auto cfg = _bus_instance.config();
        cfg.spi_host   = SPI2_HOST;
        cfg.spi_mode   = 0;
        cfg.freq_write = 65000000;
        cfg.freq_read  = 16000000;
        cfg.spi_3wire  = false;
        cfg.use_lock   = true;
        cfg.dma_channel=  1;
        cfg.dma_channel = SPI_DMA_CH_AUTO;
        cfg.pin_sclk   = 14;
        cfg.pin_mosi   = 13;
        cfg.pin_miso   = 12;
        cfg.pin_dc     =  2;

        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);
    }

    {
        auto cfg = _panel_instance.config();
        cfg.pin_cs          =    15;
        cfg.pin_rst         =    -1;
        cfg.pin_busy        =    -1;
        cfg.memory_width    =   320;
        cfg.memory_height   =   480;
        cfg.panel_width     =   320;
        cfg.panel_height    =   480;
        cfg.offset_x        =     0;
        cfg.offset_y        =     0;
        cfg.offset_rotation =     0;
        cfg.dummy_read_pixel=     8;
        cfg.dummy_read_bits =     1;
        cfg.readable        = false;
        cfg.invert          = false;
        cfg.rgb_order       = false;
        cfg.dlen_16bit      = false;
        cfg.bus_shared      = false;
        _panel_instance.config(cfg);
    }

    {
        auto cfg = _light_instance.config();
        cfg.pin_bl = 27;
        cfg.invert = false;
        cfg.freq   = 44100;
        cfg.pwm_channel = 7;
        _light_instance.config(cfg);
        _panel_instance.setLight(&_light_instance);
    }

    {
        auto cfg = _touch_instance.config();
        cfg.x_min      = 0;
        cfg.x_max      = 320;
        cfg.y_min      = 0;
        cfg.y_max      = 480;
        cfg.pin_int    = -1;
        cfg.pin_rst    = 25;
        cfg.bus_shared = false;
        // cfg.offset_rotation = 6;
        cfg.i2c_port = I2C_NUM_0;
        cfg.i2c_addr = 0x5D;
        cfg.freq = 400000;
        cfg.pin_scl = 32;
        cfg.pin_sda = 33;
        cfg.pin_cs = -1;
        _touch_instance.config(cfg);
        _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
}

static void disp_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p){
    if (tft.getStartCount()==0){  // Run if not already started
        tft.startWrite();
    } 
    tft.pushImageDMA( area->x1
                    , area->y1
                    , area->x2 - area->x1 + 1
                    , area->y2 - area->y1 + 1
                    , ( lgfx::swap565_t* )&color_p->full);
    lv_disp_flush_ready(disp);
}

// static void touchscreen_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
// {
//     bool touched;
//     uint8_t gesture;
//     uint16_t touchX, touchY;
//
//     touched = touch.getTouch(&touchX, &touchY, &gesture);
//
//     if (!touched)
//     {
//         data->state = LV_INDEV_STATE_REL;
//     }
//     else
//     {
//         data->state = LV_INDEV_STATE_PR;
//
//         /*Set the coordinates*/
//         data->point.x = touchX;
//         data->point.y = touchY;
//
//         // ESP_LOGI("TOUCH", "%d, %d", touchY, touchX);
//     }
// }

static void touchscreen_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  uint16_t touchX=0, touchY=0;
  bool touched = tft.getTouch( &touchX, &touchY);
  if(touched){
    if(touchX < TFT_WIDTH && touchY < TFT_HEIGHT){
      data->state = LV_INDEV_STATE_PR;
      data->point.x = touchX;
      data->point.y = touchY;
    }
    else{ data->state = LV_INDEV_STATE_REL;}
  }
}

// void backlight_state(bool state){
// 	gpio_set_level(BACKLIGHT_PIN, state);
// }

static void lv_tick_task(void *arg) {
    (void) arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

void gui_init(){
//     // Turn on backlight
    // gpio_set_direction(BACKLIGHT_PIN, GPIO_MODE_OUTPUT);
    // backlight_state(true);

    touch.begin();
    lv_init();
    tft.init();

    static lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(TFT_HEIGHT * 50 * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    static lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(TFT_HEIGHT * 50 * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, TFT_HEIGHT * 50);

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
