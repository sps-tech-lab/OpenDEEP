//
// Created by SPS on 28/05/2025.
//
#include "platform.hpp"
#include <cstdint>
#include <cstdio>
// #include "hardware/dma.h"
// #include "hardware/i2c.h"
// #include "hardware/pwm.h"
// #include "hardware/spi.h"
// #include "pico/stdlib.h"

// Local data
// static struct {
//     uint32_t slice;
//     int dma_channel;
// } platform;

/**
 * @brief core hardware initialization
 * @return result of initialization (not implemented)
 */
bool platform_init() {
    // //LCD gpio config
    // gpio_init(WAVESHARE_LCD_DC_PIN);
    // gpio_init(WAVESHARE_LCD_CS_PIN);
    // gpio_init(WAVESHARE_LCD_SCLK_PIN);
    // gpio_init(WAVESHARE_LCD_TX_PIN);
    // gpio_init(WAVESHARE_LCD_RST_PIN);
    // gpio_init(WAVESHARE_LCD_BL_PIN);

    // gpio_set_dir(WAVESHARE_LCD_DC_PIN,GPIO_OUT);
    // gpio_set_dir(WAVESHARE_LCD_CS_PIN,GPIO_OUT);
    // gpio_set_dir(WAVESHARE_LCD_SCLK_PIN,GPIO_OUT);
    // gpio_set_dir(WAVESHARE_LCD_TX_PIN,GPIO_OUT);
    // gpio_set_dir(WAVESHARE_LCD_RST_PIN,GPIO_OUT);
    // gpio_set_dir(WAVESHARE_LCD_BL_PIN,GPIO_OUT);

    // gpio_put(WAVESHARE_LCD_CS_PIN, true);
    // gpio_put(WAVESHARE_LCD_DC_PIN, false);
    // gpio_put(WAVESHARE_LCD_BL_PIN, true);
    // printf("[+] GPIO\r\n");

    // //PWM Config
    // gpio_set_function(WAVESHARE_LCD_BL_PIN, GPIO_FUNC_PWM);

    // platform.slice = pwm_gpio_to_slice_num(WAVESHARE_LCD_BL_PIN);

    // pwm_set_wrap(platform.slice, 100);
    // pwm_set_chan_level(platform.slice, PWM_CHAN_B, 0);
    // pwm_set_clkdiv(platform.slice, 50);
    // pwm_set_enabled(platform.slice, true);
    // printf("[+] PWM\r\n");

    // // SPI Config
    // spi_init(SPI_INSTANCE(WAVESHARE_LCD_SPI), 62500000); //Set maximal 62.5 MHz
    // gpio_set_function(WAVESHARE_LCD_SCLK_PIN, GPIO_FUNC_SPI);
    // gpio_set_function(WAVESHARE_LCD_TX_PIN, GPIO_FUNC_SPI);
    // printf("[+] SPI\r\n");

    // //I2C Config
    // i2c_init(I2C_INSTANCE(PICO_DEFAULT_I2C), 1000000);
    // gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    // gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    // gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    // gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // printf("[+] I2C\r\n");

    // printf("Initialization finished\r\n");
    return true;
}

/**
 * @brief backlight brightness level setter
 * @param level - 0..100% brightness
 */
void platform_set_backlight_level(uint8_t level) {
    // if(( level < 0 )||( level > 100 )){
    //     printf("[Error] wrong backlight_level\r\n");
    // }else{
    //     pwm_set_chan_level(platform.slice, PWM_CHAN_B, level);
    // }
}

/**
 * @brief dma channel getter
 * @return dma_channel
 */
int platform_get_dma_channel() {
    // return platform.dma_channel;
    return 0; // Draft
}
