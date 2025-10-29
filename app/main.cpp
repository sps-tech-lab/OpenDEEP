#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "GC9107.hpp"
#include "bsp.hpp"


// ---------------------------------------------------------
// Color helpers
// ---------------------------------------------------------

// Pack 8-bit RGB into RGB565
static inline uint16_t rgb888_to_565(uint8_t r, uint8_t g, uint8_t b) {
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
// Bring-up
// ---------------------------------------------------------

static void rp_periph_init(void) {
    // SPI
    spi_init(spi0, LCD_SPI_HZ);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Manual control lines
    gpio_init(PIN_CS);  gpio_set_dir(PIN_CS,  GPIO_OUT); gpio_put(PIN_CS, 1);
    gpio_init(PIN_DC);  gpio_set_dir(PIN_DC,  GPIO_OUT); gpio_put(PIN_DC, 1);
    gpio_init(PIN_RST); gpio_set_dir(PIN_RST, GPIO_OUT); gpio_put(PIN_RST, 1);

    // Backlight (off initially)
    gpio_init(PIN_BLK); gpio_set_dir(PIN_BLK, GPIO_OUT); gpio_put(PIN_BLK, 0);

    // TE input (optional)
    gpio_init(PIN_TE);
    gpio_set_dir(PIN_TE, GPIO_IN);
    gpio_pull_down(PIN_TE);  // panel will drive it; pull keeps defined level

    // Temporary 3v3 on GPIO2 (LOW CURRENT ONLY!)
    // TODO: Check profit of TE line in this project
    gpio_init(PIN_FAKE3V3);
    gpio_set_dir(PIN_FAKE3V3, GPIO_OUT);
    gpio_put(PIN_FAKE3V3, true);
}

// ---------------------------------------------------------
// Demo
// ---------------------------------------------------------
static void lcd_demo() {
    // Solid colors
    gc9107_fill_rect(&lcd, 0, 0, GC9107_WIDTH, GC9107_HEIGHT, 0xF800); // red
    sleep_ms(400);
    gc9107_fill_rect(&lcd, 0, 0, GC9107_WIDTH, GC9107_HEIGHT, 0x07E0); // green
    sleep_ms(400);
    gc9107_fill_rect(&lcd, 0, 0, GC9107_WIDTH, GC9107_HEIGHT, 0x001F); // blue
    sleep_ms(400);

    // Simple bars
    for (int y = 0; y < GC9107_HEIGHT; y += 10) {
        uint16_t c = ((y & 0x1F) << 11) | (((y*2) & 0x3F) << 5) | (y & 0x1F);
        gc9107_fill_rect(&lcd, 0, y, GC9107_WIDTH, 10, c);
    }
    sleep_ms(400);

    // Colorful bars
    for (int y = 0; y < GC9107_HEIGHT; y += 10) {
        uint8_t w = (uint8_t)((y * 255) / (GC9107_HEIGHT - 1));
        uint16_t c = wheel565(w);
        gc9107_fill_rect(&lcd, 0, y, GC9107_WIDTH, 10, c);
    }
}

// ---------------------------------------------------------
// main()
// ---------------------------------------------------------
[[noreturn]] int main() {
    stdio_init_all();
    printf("Hello, DEEP!\n");

    sleep_ms(100);

    rp_periph_init();

    // Hook up the function pointers for the driver
    lcd.gpio_cs   = rp_cs;
    lcd.gpio_dc   = rp_dc;
    lcd.gpio_rst  = rp_rst;
    lcd.spi_tx8   = rp_spi_tx8;
    lcd.spi_tx16  = rp_spi_tx16;
    lcd.delay_ms  = rp_delay;
    lcd.backlight = rp_bl;      // set to NULL if BL hardwired
    lcd.x_offset  = 0;
    lcd.y_offset  = 0;

    // Initialize display (rotation ROT_0..ROT_270)
    gc9107_init(&lcd, ROT_0);

    // Backlight on (if using PIN_BLK to gate of transistor)
    rp_bl(true);

    // Demo
    lcd_demo();

    // Idle loop
    while (true) {
        sleep_ms(1000);
    }
}