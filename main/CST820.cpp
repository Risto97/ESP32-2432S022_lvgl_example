#include "CST820.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "hal/i2c_types.h"

CST820::CST820(gpio_num_t sda_pin, gpio_num_t scl_pin, gpio_num_t rst_pin, gpio_num_t int_pin)
{
    _sda = sda_pin;
    _scl = scl_pin;
    _rst = rst_pin;
    _int = int_pin;
}

void CST820::begin(void)
{
    // int i2c_master_port = 0;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,
        .scl_io_num = 22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE
    };
    conf.master.clk_speed = 1000000;
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    // Int Pin Configuration
    if (_int != GPIO_NUM_NC)
    {
        gpio_set_direction(_int, GPIO_MODE_OUTPUT);
        gpio_set_level(_int, 1);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        gpio_set_level(_int, 0);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    // Reset Pin Configuration
    if (_rst != GPIO_NUM_NC)
    {
        gpio_set_direction(_rst, GPIO_MODE_OUTPUT);
        gpio_set_level(_rst, 0);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        gpio_set_level(_rst, 1);
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }

    // Initialize Touch
    i2c_write(0xFE, 0XFF);
}

bool CST820::getTouch(uint16_t *x, uint16_t *y, uint8_t *gesture)
{
    bool FingerIndex = false;
    FingerIndex = (bool)i2c_read(0x02);

    *gesture = i2c_read(0x01);
    if (!(*gesture == SlideUp || *gesture == SlideDown))
    {
        *gesture = None;
    }

    uint8_t data[4];
    i2c_read_continuous(0x03,data,4);
    *x = ((data[0] & 0x0f) << 8) | data[1];
    *y = ((data[2] & 0x0f) << 8) | data[3];


    return FingerIndex;
}

uint8_t CST820::i2c_read(uint8_t addr)
{
    uint8_t read_data;
    i2c_master_write_read_device(I2C_NUM_0, I2C_ADDR_CST820, &addr, 1, &read_data, 1, 1000 / portTICK_PERIOD_MS);
    return read_data;
}

uint8_t CST820::i2c_read_continuous(uint8_t addr, uint8_t *data, uint32_t length)
{
    esp_err_t ret = i2c_master_write_read_device(I2C_NUM_0, I2C_ADDR_CST820, &addr, 1, data, length, 1000 / portTICK_PERIOD_MS);
  return 0;
}

void CST820::i2c_write(uint8_t addr, uint8_t data)
{
    uint8_t send_data[2] = {addr, data};
    esp_err_t err = i2c_master_write_to_device(I2C_NUM_0, I2C_ADDR_CST820, send_data, 2, 1000 / portTICK_PERIOD_MS);
}

// uint8_t CST820::i2c_write_continuous(uint8_t addr, const uint8_t *data, uint32_t length)
// {
//   Wire.beginTransmission(I2C_ADDR_CST820);
//   Wire.write(addr);
//   for (int i = 0; i < length; i++) {
//     Wire.write(*data++);
//   }
//   if ( Wire.endTransmission(true))return -1;
//   return 0;
// }

