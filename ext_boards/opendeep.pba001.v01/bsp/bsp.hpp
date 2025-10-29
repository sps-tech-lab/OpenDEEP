//
// Created by SPS on 29/10/2025.
//
// @Board: RP2040 PiZero
#ifndef BSP_HPP
#define BSP_HPP

// ===== Display geometry =====
#define GC9107_WIDTH   128
#define GC9107_HEIGHT  115

// ===== Pin map =====
#define PIN_SCK      18  // SPI0 SCK
#define PIN_MOSI     19  // SPI0 MOSI
// MISO not used
#define PIN_CS       17
#define PIN_DC       20
#define PIN_RST      21
#define PIN_BLK      13  // Backlight control pin (HIGH=on) -> drive transistor / BL pin
#define PIN_TE       15  // TE input (tearing effect)
#define PIN_FAKE3V3   2  // Temporary 3v3 for VDD (LOW-CURRENT ONLY!)

// ===== SPI speed (Hz) =====
#define LCD_SPI_HZ   (10 * 1000 * 1000) // start at 10 MHz; raise later if stable

// ===== Optional: Wait for TE =====
// #define USE_TE_SYNC 1

// RP2040 port — SPI/GPIO
//TODO: make normal port interface
static inline void rp_spi_tx8(const uint8_t *data, uint32_t len) {
    spi_write_blocking(spi0, data, len);
}
static inline void rp_spi_tx16(const uint16_t *data, uint32_t len) {
    spi_write16_blocking(spi0, data, len);
}
static inline void rp_cs(bool level)   { gpio_put(PIN_CS,  level); }
static inline void rp_dc(bool level)   { gpio_put(PIN_DC,  level); }
static inline void rp_rst(bool level)  { gpio_put(PIN_RST, level); }
static inline void rp_delay(uint32_t ms){ sleep_ms(ms); }
static inline void rp_bl(bool on)      { gpio_put(PIN_BLK, on ? 1 : 0); }

#endif //BSP_HPP
