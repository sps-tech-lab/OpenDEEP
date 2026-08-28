//
// Created by :: SPS :: on 27/08/2026.
//
// @Board: Virtual (host simulator)
#include "bsp.hpp"
#include <cstdio>
#include "hardware/i2c.h"
#include "pico/stdlib.h"

void bsp_ps_init() {
    gpio_init(SENSOR_POWER);
    gpio_set_dir(SENSOR_POWER, GPIO_OUT);
    gpio_put(SENSOR_POWER, true);

    //TODO: init sensor

    printf("[sim] MS5837 attached to fake I2C bus\n");
}

void bsp_lcd_init() {
    //TODO: implement simulator window satart
}

void bsp_i2c_init() {
    i2c_init(BSP_I2C_PORT, BSP_I2C_HZ);
    gpio_set_function(BSP_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(BSP_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(BSP_I2C_SDA);
    gpio_pull_up(BSP_I2C_SCL);
}
