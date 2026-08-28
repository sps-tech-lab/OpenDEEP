//
// Created by :: SPS :: on 27/08/2026.
//
// @Brief:   pico-sdk/bsp stubs
// @Details: Kept apart from fake_ms5837_bus.cpp because
//           the host simulator shares that fake bus
//
#include <cstdint>
#include "bsp.hpp"
#include "pico/stdlib.h"

void sleep_ms(uint32_t /*ms*/) {
    // No delay on the host,a busy-wait would only slow the tests down
}

void bsp_ps_init() {
    // Nothing to power up on the host
}
