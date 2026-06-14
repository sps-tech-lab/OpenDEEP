//
// Created by SPS on 29/10/2025.
//
// @Board: RP2040 PiZero
#ifndef BSP_HPP
#define BSP_HPP

// ===== Display geometry =====
#define GC9107_WIDTH  128
#define GC9107_HEIGHT 115

// ===== Pin map =====
#define PIN_LCD_SCK     18
#define PIN_LCD_MOSI    19
#define PIN_LCD_CS      17
#define PIN_LCD_DC      20 // LCD Data/Command
#define PIN_LCD_RST     21 // LCD Reset
#define PIN_LCD_BLK     13 // Backlight control pin (HIGH=on) -> drive transistor / BL pin
#define PIN_LCD_TE      15 // TE input (tearing effect)
#define PIN_FAKE3V3_LCD 2  // Temporary 3v3 for LCD VDD (LOW-CURRENT ONLY!)
#define SENSOR_POWER    3  // Temporary 3v3 for MS5837 VDD (LOW-CURRENT ONLY!)

// ===== SPI =====
#define LCD_SPI_PORT spi0
#define LCD_SPI_HZ   (10 * 1000 * 1000) // start at 10 MHz; raise later if stable

// ===== I2C =====
#define BSP_I2C_PORT i2c0
#define BSP_I2C_HZ   (400 * 1000)
#define PIN_I2C_SDA  PICO_DEFAULT_I2C_SDA_PIN
#define PIN_I2C_SCL  PICO_DEFAULT_I2C_SCL_PIN

// ===== Optional: Wait for TE =====
// #define USE_TE_SYNC 1

/**
 * @brief   I2C initialization
 * @details Based on default RP2040 PiZero pinout
 */
void bsp_i2c_init();

/**
 * @brief   Pressure Sensor initialization
 */
void bsp_ps_init();

/**
 * @brief   LCD initialization
 */
void bsp_lcd_init();

#endif // BSP_HPP
