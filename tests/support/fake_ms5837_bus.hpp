//
// Created by SPS on 19/07/2026.
//
// Fake I2C bus implementation for MS5837 unit-tests
//
// This fake implements <hardware/i2c.h> helpers on the host and responds like real MS5837
//
//   * RESET (0x1E)                       -> acknowledged
//   * READ_PROM (0xA0 + 2*i)             -> next read returns calibration word i
//   * D1/D2 conversion (0x40.. / 0x50..) -> selects pressure / temperature ADC
//   * ADC_READ (0x00)                    -> next read returns selected 24-bit value
//
#ifndef OPENDEEP_TEST_FAKE_MS5837_BUS_HPP
#define OPENDEEP_TEST_FAKE_MS5837_BUS_HPP

#include <cstdint>

struct FakeMS5837 {
    uint16_t prom[8] = {0};      // calibration words C0..C7 (C0 holds part of CRC)
    uint32_t d1_pressure = 0;    // raw 24-bit pressure ADC result    (D1)
    uint32_t d2_temperature = 0; // raw 24-bit temperature ADC result (D2)
};

// Reset all fake-bus state and detach any device
void fake_bus_reset();

// Attach an emulated MS5837
void fake_bus_attach(const FakeMS5837& device);

// Inject a transfer failure
// the N-th I2C transfer returns PICO_ERROR_GENERIC
// Note: pass -1 to disable fault injection
void fake_bus_fail_on_transfer(int transfer_index);

// Total number of I2C transfers performed since the last reset/attach
int fake_bus_transfer_count();

// Fill prom[1..6] with the given coefficients, then compute and store a valid
// CRC nibble in prom[0], driver's CRC check passes. prom[7] is cleared
void fake_bus_make_valid_prom(FakeMS5837& device, uint16_t c1, uint16_t c2, uint16_t c3, uint16_t c4, uint16_t c5,
                              uint16_t c6);

#endif // OPENDEEP_TEST_FAKE_MS5837_BUS_HPP
