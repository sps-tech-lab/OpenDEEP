//
// Created by SPS on 28/05/2025.
//
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

bool platform_init();
void platform_set_backlight_level(uint8_t level);
int  platform_get_dma_channel();

#endif //PLATFORM_H
