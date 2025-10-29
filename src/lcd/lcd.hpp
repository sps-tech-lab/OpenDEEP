#ifndef __LCD_H
#define __LCD_H	

#include <stdint.h>
#include "graphics.hpp"


//Display resolution
#define LCD_HEIGHT  240
#define LCD_WIDTH   240


//Display layout
#define HORIZONTAL  0
#define VERTICAL    1


//Display class
class LCD : public FrameBuffer
{
    uint8_t direction;

    void data(uint8_t);
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
