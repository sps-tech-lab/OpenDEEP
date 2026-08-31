//
// Created by :: SPS :: on 27/08/2026.
//
// @Board:   Virtual (host simulator)
// @Details: Mirrors pin map and peripheral configuration
//           of [opendeep.pba001.cnc.v01]
//
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
#define SENSOR_POWER 3 // Power for MS5837 VDD

// ===== I2C =====
#define BSP_I2C_PORT i2c0
#define BSP_I2C_HZ   (400 * 1000)
#define BSP_I2C_SDA  4
#define BSP_I2C_SCL  5

// ===== UART =====
// Look in bsp.cmake
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

// Simulator

// ===== Virtual panel geometry =====
#define SIM_GRAM_WIDTH     128
#define SIM_GRAM_HEIGHT    128
#define SIM_PANEL_X_ORIGIN 0
#define SIM_PANEL_Y_ORIGIN 13

// ===== Panel wiring calibration =====
#define SIM_PANEL_INVERTED  1
#define SIM_PANEL_BGR_WIRED 1

// ===== Panel orientation =====
#define SIM_PANEL_MOUNT_ROTATION 180

// ===== Window =====
#define SIM_WINDOW_TITLE "OpenDEEP - Simulator"
#define SIM_WINDOW_SCALE 1

/**
 * @brief   I2C initialization
 * @details Fake MS5837 device answers regardless of bus setup
 */
void bsp_i2c_init();

/**
 * @brief   Pressure Sensor initialization
 * @details Attaches the simulated MS5837 (PROM + starting ADC codes) to the fake bus
 */
void bsp_ps_init();

/**
 * @brief   LCD initialization
 * @details Opens the simulator window and resets the virtual GC9107 panel
 */
void bsp_lcd_init();

#endif // BSP_HPP
