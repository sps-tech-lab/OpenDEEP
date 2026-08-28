//
// Created by :: SPS :: on 27/08/2026.
//
// @Board:   Virtual (host simulator)
// @Details: <hardware/i2c.h>  for host simulator
//           Match fake MS5837 device.
//
#ifndef OPENDEEP_SIM_HARDWARE_I2C_H
#define OPENDEEP_SIM_HARDWARE_I2C_H

#include <cstddef>
#include <cstdint>

typedef struct i2c_inst i2c_inst_t;

extern i2c_inst_t* const i2c0;
extern i2c_inst_t* const i2c1;

uint32_t i2c_init(i2c_inst_t* i2c, uint32_t baudrate);
int i2c_write_blocking(i2c_inst_t* i2c, uint8_t addr, const uint8_t* src, size_t len, bool nostop);
int i2c_read_blocking(i2c_inst_t* i2c, uint8_t addr, uint8_t* dst, size_t len, bool nostop);

#endif // OPENDEEP_SIM_HARDWARE_I2C_H
