//
// Created by SPS on 30/10/2025.
//
#include "pico/stdlib.h"
#include "GC9107.hpp"
#include "demo.hpp"
#include "bsp.hpp"

// ---------------------------------------------------------
// Color helpers
// ---------------------------------------------------------

// Pack 8-bit RGB into RGB565
uint16_t rgb888_to_565(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

// NeoPixel-style color wheel (0..255) → vivid RGB565
static uint16_t wheel565(uint8_t pos) {
    if (pos < 85) {
        return rgb888_to_565((uint8_t)(pos * 3), (uint8_t)(255 - pos * 3), 0);
    } else if (pos < 170) {
        pos -= 85;
        return rgb888_to_565((uint8_t)(255 - pos * 3), 0, (uint8_t)(pos * 3));
    } else {
        pos -= 170;
        return rgb888_to_565(0, (uint8_t)(pos * 3), (uint8_t)(255 - pos * 3));
    }
}


// ---------------------------------------------------------
// Demo
// ---------------------------------------------------------
void lcd_demo(LCD *_lcd) {
    // Solid colors
    _lcd->fillScreen( LCD_RED ); // red
    _lcd->update();
    sleep_ms(400);
    _lcd->fillScreen( LCD_GREEN ); // green
    _lcd->update();
    sleep_ms(400);
    _lcd->fillScreen( LCD_BLUE ); // blue
    _lcd->update();
    sleep_ms(400);

    _lcd->drawCircle(GC9107_WIDTH/2-10, GC9107_HEIGHT/2-10, 20, LCD_WHITE, 1, true);
    _lcd->update();
    sleep_ms(400);

    // Simple bars
    for (int y = 0; y < GC9107_HEIGHT; y += 10) {
        uint16_t c = ((y & 0x1F) << 11) | (((y*2) & 0x3F) << 5) | (y & 0x1F);
        _lcd->drawRect( 0, y, GC9107_WIDTH, 10, c, 1, true);;
    }
    _lcd->update();
    sleep_ms(400);

    // Colorful bars
    for (int y = 0; y < GC9107_HEIGHT; y += 10) {
        uint8_t w = (uint8_t)((y * 255) / (GC9107_HEIGHT - 1));
        uint16_t c = wheel565(w);
        _lcd->drawRect( 0, y, GC9107_WIDTH, 10, c, 1, true);;
    }
    _lcd->update();
    sleep_ms(400);
}
