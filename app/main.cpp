#include <cstdio>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico/rand.h"
#include "GC9107.hpp"
#include "bsp.hpp"
#include "demo.hpp"
#include "lcd.hpp"
#include "bitmaps.hpp"
#include "MS5837.hpp"
#include "i2c_tools.hpp"

//Framebuffer
static uint16_t fbuf[LCD_WIDTH*LCD_HEIGHT];


float round_to_screen(float value) {
    float scaled = value * 10.0f;
    float rounded_scaled = roundf(scaled);
    float result = rounded_scaled / 10.0f;
    return result;
}


// ---------------------------------------------------------
// main()
// ---------------------------------------------------------
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

    //Logo
    lcd.drawText(15,  47, &oswald_bold_24,  LCD_DARKGREY, LCD_BLACK, 2, "OpenDEEP");
    lcd.update();
    sleep_ms(1000);

    // Initialize Pressure/Temperature Sensor
    PresTempSensor pts;
    bsp_i2c_init();
    pts.init();

    //Test
    while(1)
    {
        lcd.fillScreen(LCD_BLACK);

        pts.measure();
        printf("Pressure: %.2f mbar\n", pts.pressure());
        printf("Temperature: %.2f 'C\n", pts.temperature());

        lcd.drawText(28,  28, &oswald_bold_24,  LCD_DARKGREY, LCD_BLACK, 2, "%.2f", pts.pressure());
        lcd.drawText(28,  68, &oswald_bold_24,  LCD_DARKGREY, LCD_BLACK, 2, "%.2f", pts.temperature());
        lcd.update();

        sleep_ms(100);
    };

// #define CHANGE_RANGE 100
//     float val = 9.9;
//     float lookup = 0.0;
//     uint32_t timflow = CHANGE_RANGE;
//     while(1){
//         while(--timflow != 0){
//             if( lookup < val ){
//                 lookup = round_to_screen(lookup+0.1f);
//             }else if ( lookup > val ){
//                 lookup = round_to_screen(lookup-0.1f);
//             }
//             lcd.fillScreen(LCD_BLACK);
//             uint32_t xpos = lcd.drawText(28,  28, &oswald_bold_48,  LCD_DARKGREY, LCD_BLACK, 2, "%.1f", lookup);
//             lcd.drawText(xpos,  56, &oswald_bold_20,  LCD_DARKGREY, LCD_BLACK, 2, "m");
//             lcd.update();
//             sleep_ms(1);
//             printf("lookup: %f\n", lookup);
//         }
//         val = round_to_screen((float)(get_rand_32()%99)/10);
//         timflow = CHANGE_RANGE;
//         printf("val: %f\n", val);
//     }
}

