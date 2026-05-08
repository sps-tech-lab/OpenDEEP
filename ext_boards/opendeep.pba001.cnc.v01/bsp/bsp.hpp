//
// Created by SPS on 08/05/2026.
//
// @Board: RP2040 CNC
#ifndef BSP_HPP
#define BSP_HPP

// ===== Display geometry =====
#define GC9107_WIDTH  128
#define GC9107_HEIGHT 115

// ===== Pin map =====
#define PIN_LCD_SCK     10
#define PIN_LCD_MOSI    11
#define PIN_LCD_CS      9
#define PIN_LCD_DC      14 // LCD Data/Command
#define PIN_LCD_RST     12 // LCD Reset
#define PIN_LCD_BLK     13 // Backlight control pin (HIGH=on) -> drive transistor / BL pin
#define PIN_LCD_TE      15 // TE input (tearing effect)
#define PIN_FAKE3V3_LCD 2  // Temporary 3v3 for LCD VDD    (LOW-CURRENT ONLY!)
#define PIN_FAKE3V3_MS5 3  // Temporary 3v3 for MS5837 VDD (LOW-CURRENT ONLY!)

// ===== SPI =====
#define LCD_SPI_PORT spi1
#define LCD_SPI_HZ   (10 * 1000 * 1000) // start at 10 MHz; raise later if stable

// ===== I2C =====
#define PIN_I2C_SDA PICO_DEFAULT_I2C_SDA_PIN
#define PIN_I2C_SCL PICO_DEFAULT_I2C_SCL_PIN

// ===== Optional: Wait for TE =====
// #define USE_TE_SYNC 1

/**
 * @brief   I2C initialization
 * @details Based on default RP2040 PiZero pinout
 */
void bsp_i2c_init();

#endif // BSP_HPP
