//
// Created by SPS on 30/10/2025.
//

#ifndef DEMO_HPP
#define DEMO_HPP

#include <cstdint>
#include "lcd.hpp"

// Public interface
uint16_t rgb888_to_565(uint8_t r, uint8_t g, uint8_t b);
void lcd_demo(LCD* _lcd);

#endif // DEMO_HPP
