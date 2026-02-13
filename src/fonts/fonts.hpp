#ifndef FONTS_H
#define FONTS_H

#include <cstdint>

struct font {
    const uint8_t* table;
    uint8_t width;
    uint8_t height;

    font(const uint8_t* _table, uint8_t _width, uint8_t _height)
        : table(_table), width(_width), height(_height){};
};

// Oswald fonts
extern font oswald_light_12;
extern font oswald_light_16;
extern font oswald_light_20;
extern font oswald_light_24;
extern font oswald_light_36;

extern font oswald_regular_12;
extern font oswald_regular_16;
extern font oswald_regular_20;
extern font oswald_regular_24;
extern font oswald_regular_36;

extern font oswald_medium_12;
extern font oswald_medium_16;
extern font oswald_medium_20;
extern font oswald_medium_24;
extern font oswald_medium_36;

extern font oswald_bold_12;
extern font oswald_bold_16;
extern font oswald_bold_20;
extern font oswald_bold_24;
extern font oswald_bold_36;

extern font oswald_bold_48;

#endif
