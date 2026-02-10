#ifndef __LCD_H
#define __LCD_H

#include <cstdint>
#include "bsp.hpp"
#include "graphics.hpp"

// Display resolution
#define LCD_HEIGHT GC9107_HEIGHT
#define LCD_WIDTH  GC9107_WIDTH

#define LCD_H_CENTER (GC9107_HEIGHT / 2 + 6)
#define LCD_W_CENTER (GC9107_WIDTH / 2 - 1)

// Display layout
#define HORIZONTAL 0
#define VERTICAL   1

// Display class
class LCD : public FrameBuffer {
    uint8_t direction;

    void data(uint8_t);
    void data_buf(const uint8_t*, uint8_t);
    void command(uint8_t);
    void init_reg();
    void set_window(uint16_t, uint16_t, uint16_t, uint16_t);

public:
    LCD(uint16_t*);
    LCD(uint16_t*, uint8_t);
    void reset();
    void update() override;

    void set_direction(uint8_t);
    uint8_t get_direction();
};

#endif
