//
// Created by :: SPS :: on 27/08/2026.
//
// @Board:   Virtual (host simulator)
// @Details: <pico/stdlib.h>  for host simulator
//           Mock slice of pico-sdk
//
#ifndef OPENDEEP_SIM_PICO_STDLIB_H
#define OPENDEEP_SIM_PICO_STDLIB_H

#include <cstddef>
#include <cstdint>

#ifndef PICO_OK
#define PICO_OK 0
#endif

#ifndef PICO_ERROR_GENERIC
#define PICO_ERROR_GENERIC (-1)
#endif

// GPIO directions
#define GPIO_IN  0
#define GPIO_OUT 1

// SDK gpio_function enum values
enum gpio_function {
    GPIO_FUNC_XIP = 0,
    GPIO_FUNC_SPI = 1,
    GPIO_FUNC_UART = 2,
    GPIO_FUNC_I2C = 3,
    GPIO_FUNC_PWM = 4,
    GPIO_FUNC_SIO = 5,
    GPIO_FUNC_PIO0 = 6,
    GPIO_FUNC_PIO1 = 7,
    GPIO_FUNC_GPCK = 8,
    GPIO_FUNC_USB = 9,
    GPIO_FUNC_NULL = 0x1f,
};

void sleep_ms(uint32_t ms);

bool stdio_init_all();

void gpio_init(uint32_t gpio);
void gpio_set_dir(uint32_t gpio, bool out);
void gpio_put(uint32_t gpio, bool value);
bool gpio_get(uint32_t gpio);
void gpio_set_function(uint32_t gpio, enum gpio_function fn);
void gpio_pull_up(uint32_t gpio);
void gpio_pull_down(uint32_t gpio);

#endif // OPENDEEP_SIM_PICO_STDLIB_H
