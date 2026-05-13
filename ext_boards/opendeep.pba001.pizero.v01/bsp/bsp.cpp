//
// Created by SPS on 29/10/2025.
//
// @Board: RP2040 PiZero
#include "bsp.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

void bsp_ps_init() {
    gpio_init(SENSOR_POWER);
    gpio_set_dir(SENSOR_POWER, GPIO_OUT);
    gpio_put(SENSOR_POWER, true);
}

void bsp_lcd_init() {
    // Temporary 3v3 on GPIO2 (LOW CURRENT ONLY!)
    gpio_init(PIN_FAKE3V3_LCD);
    gpio_set_dir(PIN_FAKE3V3_LCD, GPIO_OUT);
    gpio_put(PIN_FAKE3V3_LCD, true);
}

void bsp_i2c_init() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C_SDA);
    gpio_pull_up(PIN_I2C_SCL);
}