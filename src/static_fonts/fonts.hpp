#ifndef __FONTS_H
#define __FONTS_H

#include <stdint.h>

struct font
{
    const uint8_t *table;
    uint8_t width;
    uint8_t height;

    font(const uint8_t* _table, uint8_t _width, uint8_t _height): table(_table), width(_width), height(_height){};
};

//Default fonts
extern font font24;
extern font font20;
extern font font16;
extern font font12;

//Oswald fonts
extern font oswald_bold_12;
extern font oswald_bold_20;


#endif
