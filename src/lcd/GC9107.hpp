//
// Created by SPS on 29/10/2025.
//

// DCS commands
#define GC9107_SWRESET  0x01
#define GC9107_SLPIN    0x10
#define GC9107_SLPOUT   0x11
#define GC9107_INVOFF   0x20
#define GC9107_INVON    0x21
#define GC9107_DISPOFF  0x28
#define GC9107_DISPON   0x29
#define GC9107_CASET    0x2A
#define GC9107_RASET    0x2B
#define GC9107_RAMWR    0x2C
#define GC9107_MADCTL   0x36
#define GC9107_COLMOD   0x3A

// MADCTL rotation presets (RGB order).
#define ROT_0    0x00
#define ROT_90   0x60
#define ROT_180  0xC0
#define ROT_270  0xA0


// Tiny GC9107 "driver" struct
typedef struct {
    // GPIO/SPI hooks
    void (*gpio_cs)(bool level);
    void (*gpio_dc)(bool level);
    void (*gpio_rst)(bool level);
    void (*spi_tx8)(const uint8_t *data, uint32_t len);
    void (*spi_tx16)(const uint16_t* data, uint32_t len);
    void (*delay_ms)(uint32_t ms);
    void (*backlight)(bool on);
    // alignment shifts
    uint16_t x_offset;
    uint16_t y_offset;
} gc9107_t;

// Global LCD instance
static gc9107_t lcd;

// Low-level write helpers
static inline void lcd_cmd(gc9107_t *l, uint8_t c) {
    l->gpio_dc(false);
    l->gpio_cs(false);
    l->spi_tx8(&c, 1);
    l->gpio_cs(true);
}
static inline void lcd_data8(gc9107_t *l, uint8_t v) {
    l->gpio_dc(true);
    l->gpio_cs(false);
    l->spi_tx8(&v, 1);
    l->gpio_cs(true);
}
static inline void lcd_data(gc9107_t *l, const uint8_t *buf, uint32_t n) {
    l->gpio_dc(true);
    l->gpio_cs(false);
    l->spi_tx8(buf, n);
    l->gpio_cs(true);
}

//Public interface
void gc9107_set_addr_window(gc9107_t *l, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void gc9107_fill_rect(gc9107_t *l, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void gc9107_init(gc9107_t *l, uint8_t rotation);
