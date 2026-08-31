//
// Created by :: SPS :: on 29/08/2026.
//
// @Brief: SDL2 simulator window
//
#ifndef OPENDEEP_SIM_WINDOW_HPP
#define OPENDEEP_SIM_WINDOW_HPP

#include <cstdint>

/**
 * @brief Create the simulator window
 * @param scale Integer magnification
 */
void sim_window_open(const char* title, int width, int height, int scale);

/**
 * @brief Finished panel frame
 * @param backlight Drawn dimmed when the backlight GPIO is low
 */
void sim_window_present(const uint16_t* rgb565, int width, int height, bool backlight);

/**
 * @brief Event queue
 * @details Called from sleep_ms(), which is what keeps window responsive
 *          while the firmware's main loop runs untouched
 */
void sim_window_pump();

/**
 * @brief Wall-clock time
 * @details Backed by SDL_Delay rather than std::this_thread
 */
void sim_window_delay(uint32_t ms);

// Keyboard state
float sim_input_depth_bias();
bool sim_input_paused();

#endif // OPENDEEP_SIM_WINDOW_HPP
