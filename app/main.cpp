#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "GC9107.hpp"


// ---------------------------------------------------------
// GC9107 minimal driver
// ---------------------------------------------------------

// ===== Display geometry =====
#define GC9107_WIDTH   128
#define GC9107_HEIGHT  115

// ===== Pin map =====
#define PIN_SCK      18  // SPI0 SCK
#define PIN_MOSI     19  // SPI0 MOSI
// MISO not used
#define PIN_CS       17
#define PIN_DC       20
#define PIN_RST      21
#define PIN_BLK      13  // Backlight control pin (HIGH=on) -> drive transistor / BL pin
#define PIN_TE       15  // TE input (tearing effect)
#define PIN_FAKE3V3   2  // Temporary 3v3 for VDD (LOW-CURRENT ONLY!)

// ===== SPI speed (Hz) =====
#define LCD_SPI_HZ   (10 * 1000 * 1000) // start at 10 MHz; raise later if stable

// ===== Optional: Wait for TE =====
// #define USE_TE_SYNC 1

// ---------------------------------------------------------
// Tiny GC9107 "driver"
// ---------------------------------------------------------
typedef struct {
    // GPIO/SPI hooks
    void (*gpio_cs)(bool level);
    void (*gpio_dc)(bool level);
    void (*gpio_rst)(bool level);
    void (*spi_tx)(const uint8_t *data, uint32_t len);
    void (*delay_ms)(uint32_t ms);
    void (*backlight)(bool on);
    // alignment shifts
    uint16_t x_offset;
    uint16_t y_offset;
} gc9107_t;



// RP2040 port — SPI/GPIO
static inline void rp_spi_tx(const uint8_t *data, uint32_t len) {
    spi_write_blocking(spi0, data, len);
}
static inline void rp_cs(bool level)   { gpio_put(PIN_CS,  level); }
static inline void rp_dc(bool level)   { gpio_put(PIN_DC,  level); }
static inline void rp_rst(bool level)  { gpio_put(PIN_RST, level); }
static inline void rp_delay(uint32_t ms){ sleep_ms(ms); }
static inline void rp_bl(bool on)      { gpio_put(PIN_BLK, on ? 1 : 0); }

// Low-level write helpers
static inline void lcd_cmd(gc9107_t *l, uint8_t c) {
    l->gpio_dc(false);
    l->gpio_cs(false);
    l->spi_tx(&c, 1);
    l->gpio_cs(true);
}
static inline void lcd_data8(gc9107_t *l, uint8_t v) {
    l->gpio_dc(true);
    l->gpio_cs(false);
    l->spi_tx(&v, 1);
    l->gpio_cs(true);
}
static inline void lcd_data(gc9107_t *l, const uint8_t *buf, uint32_t n) {
    l->gpio_dc(true);
    l->gpio_cs(false);
    l->spi_tx(buf, n);
    l->gpio_cs(true);
}

// Address window
static void gc9107_set_addr_window(gc9107_t *l, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    uint16_t x0 = x + l->x_offset, x1 = x0 + w - 1;
    uint16_t y0 = y + l->y_offset, y1 = y0 + h - 1;

    uint8_t ca[4] = { (uint8_t)(x0>>8), (uint8_t)x0, (uint8_t)(x1>>8), (uint8_t)x1 };
    uint8_t ra[4] = { (uint8_t)(y0>>8), (uint8_t)y0, (uint8_t)(y1>>8), (uint8_t)y1 };

    lcd_cmd(l, GC9107_CASET); lcd_data(l, ca, 4);
    lcd_cmd(l, GC9107_RASET); lcd_data(l, ra, 4);
    lcd_cmd(l, GC9107_RAMWR);
}

// Push a solid rectangle (RGB565 color)
static void gc9107_fill_rect(gc9107_t *l, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
#ifdef USE_TE_SYNC
    // sync to TE rising edge, if enabled
#endif
    gc9107_set_addr_window(l, x, y, w, h);
    l->gpio_dc(true); l->gpio_cs(false);
    uint8_t px[2] = { (uint8_t)(color>>8), (uint8_t)color };
    uint32_t count = (uint32_t)w * h;
    for (uint32_t i = 0; i < count; ++i) {
        l->spi_tx(px, 2);
    }
    l->gpio_cs(true);
}

// Basic init
static void gc9107_init(gc9107_t *l, uint8_t rotation) {
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
// RP2040 bring-up
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
    gpio_put(PIN_FAKE3V3, 1);
}

// Global LCD instance
static gc9107_t lcd;

// ---------------------------------------------------------
// Demo
// ---------------------------------------------------------
static void lcd_demo(void) {
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
int main() {
    stdio_init_all();
    printf("Hello, DEEP!\n");

    sleep_ms(100);

    rp_periph_init();

    // Hook up the function pointers for the driver
    lcd.gpio_cs   = rp_cs;
    lcd.gpio_dc   = rp_dc;
    lcd.gpio_rst  = rp_rst;
    lcd.spi_tx    = rp_spi_tx;
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