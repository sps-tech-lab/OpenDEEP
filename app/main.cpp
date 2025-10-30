#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico/rand.h"
#include "GC9107.hpp"
#include "bsp.hpp"
#include "demo.hpp"
#include "lcd.hpp"

//Framebuffer
static uint16_t fbuf[LCD_WIDTH*LCD_HEIGHT];


// ---------------------------------------------------------
// main()
// ---------------------------------------------------------
[[noreturn]] int main() {
    stdio_init_all();
    printf("Hello, DEEP!\n");

    sleep_ms(100);

    // Initialize display
    gc9107_init();

    LCD lcd((uint16_t *) &fbuf, ROT_180);

    lcd.fillScreen(LCD_BLACK);
    lcd.update();

    // Backlight on (if using PIN_BLK to gate of transistor)
    gpio_put(PIN_BLK, true);

    // Demo
    //lcd_demo(&lcd);


    // Idle loop
    while (true) {

        lcd.fillScreen(LCD_BLACK);

        lcd.drawLine(LCD_W_CENTER, 0, LCD_W_CENTER, GC9107_HEIGHT, LCD_DARKGREEN, 1);
        lcd.drawLine(0, LCD_H_CENTER, GC9107_WIDTH, LCD_H_CENTER, LCD_DARKGREEN, 1);

        lcd.drawPoint(get_rand_32()%GC9107_WIDTH,get_rand_32()%GC9107_HEIGHT, LCD_RED, get_rand_32()%5 );
        lcd.drawPoint(get_rand_32()%GC9107_WIDTH,get_rand_32()%GC9107_HEIGHT, LCD_RED, get_rand_32()%5 );
        lcd.drawPoint(get_rand_32()%GC9107_WIDTH,get_rand_32()%GC9107_HEIGHT, LCD_RED, get_rand_32()%5 );

        lcd.drawCircle(LCD_W_CENTER, LCD_H_CENTER, 20, LCD_DARKGREEN, 1, true);
        lcd.drawCircle(LCD_W_CENTER, LCD_H_CENTER, 40, LCD_DARKGREEN, 1, false);
        lcd.drawCircle(LCD_W_CENTER, LCD_H_CENTER, 60, LCD_DARKGREEN, 1, false);
        lcd.drawCircle(LCD_W_CENTER, LCD_H_CENTER, 100, LCD_DARKGREEN, 1, false);
        lcd.drawText(45, LCD_HEIGHT/2, "SPS", &oswald_bold_12, LCD_BLACK, LCD_DARKGREEN);

        lcd.update();
        sleep_ms(100);
    }
}