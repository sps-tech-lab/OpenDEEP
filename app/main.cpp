#include <cstdio>
#include "GC9107.hpp"
#include "MS5837.hpp"
#include "bitmaps.hpp"
#include "bsp.hpp"
#include "demo.hpp"
#include "lcd.hpp"
#include "pico/stdlib.h"

// Framebuffer
static uint16_t fbuf[LCD_WIDTH * LCD_HEIGHT];

[[noreturn]] int main() {
    stdio_init_all();
    printf("Hello, DEEP!\n");

    sleep_ms(100);

    // Initialize display
    gc9107_init();
    LCD lcd(reinterpret_cast<uint16_t*>(&fbuf), ROT_180);
    lcd.fillScreen(LCD_BLACK);
    lcd.update();

    // Backlight on
    gpio_put(PIN_BLK, true);

    // Logo
    lcd.drawText(15, 47, &oswald_bold_24, LCD_DARKGREY, LCD_BLACK, 2, "OpenDEEP");
    lcd.update();
    sleep_ms(1000);

    // Initialize Pressure/Temperature Sensor
    PresTempSensor pts;
    bsp_i2c_init();
    pts.init();

    while( true ) {
        lcd.fillScreen(LCD_BLACK);

        pts.measure();
        printf("Depth: %.1f m\n", pts.depth());
        printf("Pressure: %.2f mbar\n", pts.pressure());
        printf("Temperature: %.2f 'C\n", pts.temperature());

        uint32_t xpos = lcd.drawText(30, 28, &oswald_bold_48, LCD_DARKGREY, LCD_BLACK, 2, "%.1f", pts.depth());
        lcd.drawText(xpos, 56, &oswald_bold_20, LCD_DARKGREY, LCD_BLACK, 2, "m");
        xpos = lcd.drawText(38, 90, &oswald_medium_16, RGB565(10, 10, 50), LCD_BLACK, 2, "%.2f", pts.temperature());
        lcd.drawText(xpos, 90, &oswald_medium_16, RGB565(10, 10, 50), LCD_BLACK, 2, "'C");
        lcd.update();

        sleep_ms(100);
    };
}
