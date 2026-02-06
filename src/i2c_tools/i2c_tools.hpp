//
// Created by :: SPS :: on 04.02.2026.
//
#ifndef I2C_PORT_HPP
#define I2C_PORT_HPP

#include "lcd.hpp"

class I2C_Tools {
public:

    I2C_Tools(uint16_t x, uint16_t y, uint16_t size, uint16_t gap);
    ~I2C_Tools() = default;

    /**
     * @brief   Scan all available I2C addresses
     * @details
     */
    void scanner(LCD *lcd) const;
private:
    uint16_t x, y;  // Scanner's grid start point
    uint16_t size;  // Size of each address-cell
    uint16_t gap;   // Gap between cells
};

#endif //I2C_PORT_HPP
