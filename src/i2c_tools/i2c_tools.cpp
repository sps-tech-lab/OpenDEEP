//
// Created by :: SPS :: on 04.02.2026.
//

#include "i2c_tools.hpp"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lcd.hpp"
#include <cstdio>

// Addresses of the form 000 0xxx or 111 1xxx are reserved. No slave should
// have these addresses.
static constexpr bool i2c_reserved_addr(uint8_t addr) {
    return ((addr & 0x78u) == 0u) || ((addr & 0x78u) == 0x78u);
}


I2C_Tools::I2C_Tools(uint16_t x, uint16_t y, uint16_t size, uint16_t gap)
    : x(x), y(y), size(size), gap(gap)
{}


void I2C_Tools::scanner(LCD* lcd) const
{
    lcd->fillScreen(LCD_BLACK);
    lcd->drawText(80, 25, "I2C TEST", &oswald_light_12, LCD_DARKGREY, LCD_BLACK);
    lcd->drawText(this->x+2, this->y-15, "0123456789ABCDEF", &oswald_light_12, LCD_DARKGREY, LCD_BLACK);
    lcd->update();

    char str[10];
    uint8_t row = 0;

    for (uint8_t addr = 0x00; addr < (1 << 7); ++addr) {
        snprintf(str, sizeof(str), "0x%02x", addr);
        lcd->drawText(55, 95, str, &oswald_light_12, LCD_DARKGREY, LCD_BLACK);
        snprintf(str, sizeof(str), "%d", row);
        lcd->drawText(this->x-15, this->y+this->size*(row)-2, str, &oswald_light_12, LCD_DARKGREY, LCD_BLACK);
        lcd->update();

        // Perform a 1-byte dummy read from the probe address. If a slave acknowledges this address, the function
        // returns the number of bytes transferred. If the address byte is ignored, the function returns -1.

        // Skip over any reserved addresses.
        int8_t  ret;
        uint8_t rxdata;

        if (i2c_reserved_addr(addr)) {
            ret = PICO_ERROR_GENERIC;
        }else {
            ret = i2c_read_blocking(i2c1, addr, &rxdata, 1, false);
        }

        lcd->drawRect(this->x+this->size*(addr&0x0F)+this->gap,
                 this->y+this->size*(row)+this->gap,
                 this->x+this->size+this->size*(addr&0x0F),
                 this->y+this->size+this->size*(row),
                 (ret < 0 ? LCD_CYAN : LCD_MAGENTA), 1,true);
        lcd->update();
        if(addr % 16 == 15){
            row++;
        }
        sleep_ms(30);
    }
}


