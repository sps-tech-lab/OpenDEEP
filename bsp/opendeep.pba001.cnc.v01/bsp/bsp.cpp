//
// Created by SPS on 08/05/2026.
//
// @Board: RP2040 CNC
#include "bsp.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

void bsp_ps_init() {
    gpio_init(SENSOR_POWER);
    gpio_set_dir(SENSOR_POWER, GPIO_OUT);
    gpio_put(SENSOR_POWER, true);
}

void bsp_lcd_init() {
    // 3v3 is directly connected now
}

void bsp_i2c_init() {
    i2c_init(BSP_I2C_PORT, 400 * 1000);
    gpio_set_function(BSP_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(BSP_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(BSP_I2C_SDA);
    gpio_pull_up(BSP_I2C_SCL);
}