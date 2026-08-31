//
// Created by :: SPS :: on 28/08/2026.
//
// @brief: pico-sdk host mock
//
#include <cstdint>
#include <cstdio>
#include "bsp.hpp"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "sim_panel.hpp"
#include "sim_sensor.hpp"
#include "sim_window.hpp"

namespace {

constexpr uint32_t kGpioCount = 32;
bool g_gpio_level[kGpioCount] = {false};

int g_spi0_token = 0;
int g_spi1_token = 0;
int g_i2c0_token = 0;
int g_i2c1_token = 0;

} // namespace

spi_inst_t* const spi0 = reinterpret_cast<spi_inst_t*>(&g_spi0_token);
spi_inst_t* const spi1 = reinterpret_cast<spi_inst_t*>(&g_spi1_token);
i2c_inst_t* const i2c0 = reinterpret_cast<i2c_inst_t*>(&g_i2c0_token);
i2c_inst_t* const i2c1 = reinterpret_cast<i2c_inst_t*>(&g_i2c1_token);

// ---- Timing --------------------------------------------------------------

void sleep_ms(uint32_t ms) {
    sim_window_pump();
    sim_sensor_advance(ms);

    if( ms > 0 ) {
        sim_window_delay(ms);
    }
}

// ---- stdio ---------------------------------------------------------------

bool stdio_init_all() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    printf("[sim] OpenDEEP host simulator - board opendeep.sim001.mock.v01\n");
    return true;
}

// ---- GPIO ----------------------------------------------------------------

void gpio_init(uint32_t gpio) {
    if( gpio < kGpioCount ) {
        g_gpio_level[gpio] = false;
    }
}

void gpio_set_dir(uint32_t /*gpio*/, bool /*out*/) {
    //no effect on simulated peripheral
}

void gpio_put(uint32_t gpio, bool value) {
    if( gpio < kGpioCount ) {
        g_gpio_level[gpio] = value;
    }

    switch( gpio ) {
    case PIN_LCD_CS:
        sim_panel_set_cs(value);
        break;
    case PIN_LCD_DC:
        sim_panel_set_dc(value);
        break;
    case PIN_LCD_RST:
        sim_panel_set_reset(value);
        break;
    case PIN_LCD_BLK:
        sim_panel_set_backlight(value);
        break;
    default:
        break;
    }
}

bool gpio_get(uint32_t gpio) {
    return (gpio < kGpioCount) ? g_gpio_level[gpio] : false;
}

void gpio_set_function(uint32_t /*gpio*/, enum gpio_function /*fn*/) {
    // There is no pin muxing!
}

void gpio_pull_up(uint32_t /*gpio*/) {}

void gpio_pull_down(uint32_t /*gpio*/) {}

// ---- SPI -----------------------------------------------------------------

uint32_t spi_init(spi_inst_t* /*spi*/, uint32_t baudrate) {
    return baudrate;
}

void spi_set_format(spi_inst_t* /*spi*/, uint32_t /*data_bits*/, spi_cpol_t /*cpol*/, spi_cpha_t /*cpha*/,
                    spi_order_t /*order*/) {
    // The panel decodes 8-/16-bit bursts from the call used, not from a latched format register
}

int spi_write_blocking(spi_inst_t* /*spi*/, const uint8_t* src, size_t len) {
    sim_panel_write8(src, len);
    return static_cast<int>(len);
}

int spi_write16_blocking(spi_inst_t* /*spi*/, const uint16_t* src, size_t len) {
    sim_panel_write16(src, len);
    return static_cast<int>(len);
}

// ---- I2C -----------------------------------------------------------------

uint32_t i2c_init(i2c_inst_t* /*i2c*/, uint32_t baudrate) {
    // i2c_write_blocking / i2c_read_blocking come from the shared fake MS5837
    return baudrate;
}
