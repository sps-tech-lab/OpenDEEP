//
// Host test stub for <hardware/i2c.h>.
// Mirrors the signatures of the blocking I2C helpers used by the drivers.
// The implementation is provided by the fake I2C device (see support/).
//
#ifndef OPENDEEP_TEST_STUB_HARDWARE_I2C_H
#define OPENDEEP_TEST_STUB_HARDWARE_I2C_H

#include <cstddef>
#include <cstdint>

typedef struct i2c_inst i2c_inst_t;

// Return value matches the Pico SDK contract: number of bytes transferred on
// success, or a negative error code (PICO_ERROR_GENERIC) on failure.
int i2c_write_blocking(i2c_inst_t* i2c, uint8_t addr, const uint8_t* src, size_t len, bool nostop);
int i2c_read_blocking(i2c_inst_t* i2c, uint8_t addr, uint8_t* dst, size_t len, bool nostop);

#endif // OPENDEEP_TEST_STUB_HARDWARE_I2C_H
