//
// Created by SPS on 29/10/2025.
//
// @Board: RP2040 PiZero
#include "bsp.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

void bsp_i2c_init() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C_SDA);
    gpio_pull_up(PIN_I2C_SCL);
}