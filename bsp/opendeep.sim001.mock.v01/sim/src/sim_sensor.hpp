//
// Created by :: SPS :: on 30/08/2026.
//
// @Brief:  Simulated MS5837 dive profile
// @Detail: Drives the shared fake I2C device
//
#ifndef OPENDEEP_SIM_SENSOR_HPP
#define OPENDEEP_SIM_SENSOR_HPP

#include <cstdint>

/**
 * @brief Attach the simulated MS5837 to the fake bus
 * @details Called from bsp_ps_init()
 */
void sim_sensor_init();

/**
 * @brief Step the dive profile
 * @param elapsed_ms Virtual time consumed since the last call
 */
void sim_sensor_advance(uint32_t elapsed_ms);

#endif // OPENDEEP_SIM_SENSOR_HPP
