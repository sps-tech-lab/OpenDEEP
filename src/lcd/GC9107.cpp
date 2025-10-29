//
// Created by SPS on 29/10/2025.
//

#include <cstdio>
#include <cstdint>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "GC9107.hpp"
#include "bsp.hpp"

// Address window
void gc9107_set_addr_window(gc9107_t *l, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    uint16_t x0 = x + l->x_offset, x1 = x0 + w - 1;
    uint16_t y0 = y + l->y_offset, y1 = y0 + h - 1;

    uint8_t ca[4] = { (uint8_t)(x0>>8), (uint8_t)x0, (uint8_t)(x1>>8), (uint8_t)x1 };
    uint8_t ra[4] = { (uint8_t)(y0>>8), (uint8_t)y0, (uint8_t)(y1>>8), (uint8_t)y1 };

    lcd_cmd(l, GC9107_CASET); lcd_data(l, ca, 4);
    lcd_cmd(l, GC9107_RASET); lcd_data(l, ra, 4);
    lcd_cmd(l, GC9107_RAMWR);
}

// Push a solid rectangle (RGB565 color)
void gc9107_fill_rect(gc9107_t *l, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
#ifdef USE_TE_SYNC
    // sync to TE rising edge, if enabled
#endif
    gc9107_set_addr_window(l, x, y, w, h);
    l->gpio_dc(true); l->gpio_cs(false);
    uint8_t px[2] = { (uint8_t)(color>>8), (uint8_t)color };
    uint32_t count = (uint32_t)w * h;
    for (uint32_t i = 0; i < count; ++i) {
        l->spi_tx8(px, 2);
    }
    l->gpio_cs(true);
}

// Basic init
void gc9107_init(gc9107_t *l, uint8_t rotation) {
    // HW reset
    l->gpio_rst(false); l->delay_ms(10);
    l->gpio_rst(true);  l->delay_ms(120);

    // SW reset
    lcd_cmd(l, GC9107_SWRESET);
    l->delay_ms(120);

    // Sleep out
    lcd_cmd(l, GC9107_SLPOUT);
    l->delay_ms(120);

    // Pixel format: 16bpp (RGB565)
    lcd_cmd(l, GC9107_COLMOD); lcd_data8(l, 0x55);

    // Orientation (RGB)
    uint8_t madctl = rotation | 0x08; // (0x08) for BGR swap
    lcd_cmd(l, GC9107_MADCTL); lcd_data8(l, madctl);

    // Inversion ON
    lcd_cmd(l, GC9107_INVON);

    // Full window
    lcd_cmd(l, GC9107_CASET); lcd_data8(l, 0x00); lcd_data8(l, 0x00); lcd_data8(l, 0x00); lcd_data8(l, GC9107_WIDTH  - 1);
    lcd_cmd(l, GC9107_RASET); lcd_data8(l, 0x00); lcd_data8(l, 0x00); lcd_data8(l, 0x00); lcd_data8(l, GC9107_HEIGHT - 1);

    // Display on
    lcd_cmd(l, GC9107_DISPON);
    l->delay_ms(50);

    if (l->backlight) l->backlight(true);
}