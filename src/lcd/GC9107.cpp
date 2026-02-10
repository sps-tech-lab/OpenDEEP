//
// Created by SPS on 29/10/2025.
//

#include "GC9107.hpp"
#include <cstdint>
#include <cstdio>
#include "bsp.hpp"
#include "hardware/spi.h"
#include "pico/stdlib.h"

// Peripherals init
static void rp_periph_init(void) {
    // SPI
    spi_init(spi0, LCD_SPI_HZ);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Manual control lines
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    gpio_init(PIN_DC);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_put(PIN_DC, 1);
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    gpio_put(PIN_RST, 1);

    // Backlight (off initially)
    gpio_init(PIN_BLK);
    gpio_set_dir(PIN_BLK, GPIO_OUT);
    gpio_put(PIN_BLK, 0);

    // TE input (optional)
    gpio_init(PIN_TE);
    gpio_set_dir(PIN_TE, GPIO_IN);
    gpio_pull_down(PIN_TE); // panel will drive it; pull keeps defined level

    // Temporary 3v3 on GPIO2 (LOW CURRENT ONLY!)
    // TODO: Check profit of TE line in this project
    gpio_init(PIN_FAKE3V3_LCD);
    gpio_set_dir(PIN_FAKE3V3_LCD, GPIO_OUT);
    gpio_put(PIN_FAKE3V3_LCD, true);
}

// Basic init
void gc9107_init() {
    rp_periph_init();

    GC9107.x_offset = 0;
    GC9107.y_offset = 0;
}