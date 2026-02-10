//
// Created by SPS on 29/10/2025.
//

// DCS commands
#define GC9107_SWRESET 0x01
#define GC9107_SLPIN   0x10
#define GC9107_SLPOUT  0x11
#define GC9107_INVOFF  0x20
#define GC9107_INVON   0x21
#define GC9107_DISPOFF 0x28
#define GC9107_DISPON  0x29
#define GC9107_CASET   0x2A
#define GC9107_RASET   0x2B
#define GC9107_RAMWR   0x2C
#define GC9107_MADCTL  0x36
#define GC9107_COLMOD  0x3A

// MADCTL rotation presets (RGB order).
#define ROT_0   0x00
#define ROT_90  0x60
#define ROT_180 0xC0
#define ROT_270 0xA0

// Tiny GC9107 struct
typedef struct {
    // alignment shifts
    uint16_t x_offset;
    uint16_t y_offset;
    uint16_t rotation;
} gc9107_t;

// Global LCD instance
static gc9107_t GC9107;

void gc9107_init();
