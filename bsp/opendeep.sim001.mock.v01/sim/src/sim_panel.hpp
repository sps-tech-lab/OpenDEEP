//
// Created by :: SPS :: on 29/08/2026.
//
// @Brief:  Virtual GC9107 display controller
// @detail: Consumes the same SPI byte stream the real panel
//
#ifndef OPENDEEP_SIM_PANEL_HPP
#define OPENDEEP_SIM_PANEL_HPP

#include <cstddef>
#include <cstdint>

void sim_panel_set_cs(bool level);
void sim_panel_set_dc(bool level);
void sim_panel_set_reset(bool level);
void sim_panel_set_backlight(bool on);

void sim_panel_write8(const uint8_t* data, size_t len);
void sim_panel_write16(const uint16_t* data, size_t len);

#endif // OPENDEEP_SIM_PANEL_HPP
