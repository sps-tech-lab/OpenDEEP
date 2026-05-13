//
// Created by SPS on 08/05/2026.
//
// @Board: RP2040 CNC
#ifndef BSP_HPP
#define BSP_HPP

// ===== LCD Config =====
#define GC9107_WIDTH  128
#define GC9107_HEIGHT 115
// #define USE_TE_SYNC  1 // Optional: Wait for TE

// ===== LCD gpio map =====
#define PIN_LCD_SCK  10 // LCD SCL
#define PIN_LCD_MOSI 11 // LCD SDA
#define PIN_LCD_CS   9
#define PIN_LCD_DC   14 // LCD Data/Command
#define PIN_LCD_RST  12 // LCD Reset
#define PIN_LCD_BLK  2  // Backlight control pin (HIGH=on) -> drive transistor / BL pin
#define PIN_LCD_TE   15 // TE input (tearing effect)

// ===== LCD SPI config =====
#define LCD_SPI_PORT spi1
#define LCD_SPI_HZ   (10 * 1000 * 1000) // start at 10 MHz; raise later if stable

// ===== PRESSURE SENSOR =====
#define SENSOR_POWER 8 // Power for MS5837 VDD

// ===== I2C =====
#define BSP_I2C_PORT i2c1
#define BSP_I2C_HZ   (400 * 1000)
#define BSP_I2C_SDA  PICO_DEFAULT_I2C_SDA_PIN
#define BSP_I2C_SCL  PICO_DEFAULT_I2C_SCL_PIN

// ===== UART =====
// Look in ext_board.cmake
#if PICO_DEFAULT_UART == 0
#define BSP_UART_PORT uart0
#elif PICO_DEFAULT_UART == 1
#define BSP_UART_PORT uart1
#else
#error "Unsupported PICO_DEFAULT_UART value <Expected 0 or 1>"
#endif
#define BSP_UART_TX_PIN   PICO_DEFAULT_UART_TX_PIN
#define BSP_UART_RX_PIN   PICO_DEFAULT_UART_RX_PIN
#define BSP_UART_BAUDRATE PICO_DEFAULT_UART_BAUD_RATE

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
