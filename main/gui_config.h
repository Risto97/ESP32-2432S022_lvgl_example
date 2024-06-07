#ifndef __GUI_CONFIG_H_
#define __GUI_CONFIG_H_

#include "CST820.h"
#include "hal/gpio_types.h"
#include "lvgl.h"

#define TFT_WIDTH 320
#define TFT_HEIGHT 480

constexpr gpio_num_t BACKLIGHT_PIN = GPIO_NUM_0;
constexpr gpio_num_t TOUCH_I2C_SDA_PIN = GPIO_NUM_21;
constexpr gpio_num_t TOUCH_I2C_SCL_PIN = GPIO_NUM_22;


#define LGFX_USE_V1      // set to use new version of library
#include <LovyanGFX.hpp> // main library

void gui_init();
void backlight_state(bool state);

class LGFX : public lgfx::LGFX_Device
{
private:

    lgfx::Panel_ST7796 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM     _light_instance;
    lgfx::Touch_GT911 _touch_instance;

public:
    LGFX(void);
};


#endif // !__GUI_CONFIG_H_
