//
// Host test stub for the board support package header.
// The MS5837 driver only needs the I2C port handle and the sensor power-up
// hook; both are neutralised for host testing.
//
#ifndef OPENDEEP_TEST_STUB_BSP_HPP
#define OPENDEEP_TEST_STUB_BSP_HPP

#include "hardware/i2c.h"

#define BSP_I2C_PORT (reinterpret_cast<i2c_inst_t*>(0))

void bsp_ps_init();

#endif // OPENDEEP_TEST_STUB_BSP_HPP
