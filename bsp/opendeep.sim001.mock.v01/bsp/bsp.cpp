//
// Created by :: SPS :: on 27/08/2026.
//
// @Board: Virtual (host simulator)
#include "bsp.hpp"
#include <cstdio>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "sim_sensor.hpp"
#include "sim_window.hpp"

void bsp_ps_init() {
    gpio_init(SENSOR_POWER);
    gpio_set_dir(SENSOR_POWER, GPIO_OUT);
    gpio_put(SENSOR_POWER, true);

    sim_sensor_init();

    printf("[sim] MS5837 attached to fake I2C bus\n");
}

void bsp_lcd_init() {
    //Simulator window start
    sim_window_open(SIM_WINDOW_TITLE, GC9107_WIDTH, GC9107_HEIGHT, SIM_WINDOW_SCALE);
}

void bsp_i2c_init() {
    i2c_init(BSP_I2C_PORT, BSP_I2C_HZ);
    gpio_set_function(BSP_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(BSP_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(BSP_I2C_SDA);
    gpio_pull_up(BSP_I2C_SCL);
}
