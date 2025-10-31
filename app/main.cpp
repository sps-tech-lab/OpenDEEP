#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico/rand.h"
#include "GC9107.hpp"
#include "bsp.hpp"
#include "demo.hpp"
#include "lcd.hpp"
#include "bitmaps.hpp"

//Framebuffer
static uint16_t fbuf[LCD_WIDTH*LCD_HEIGHT];

struct object{
    uint32_t timeout;
    uint8_t  x;
    uint8_t  y;
    uint8_t  size;
    uint16_t color;
};

void object_processing( LCD *lcd, struct object *obj ){
    if( obj->timeout == 0 ){
        obj->x       = get_rand_32()%GC9107_WIDTH;
        obj->y       = get_rand_32()%GC9107_HEIGHT;
        obj->size    = get_rand_32()%6;
        obj->timeout = get_rand_32()%200;
    }else{
        obj->timeout--;
    }
    ( obj->timeout < 100 )?(obj->color = RGB565(obj->timeout, 0, 0)):(obj->color = RGB565(100, 0, 0));
    lcd->drawCircle(obj->x, obj->y, obj->size, obj->color, 1, true);
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

    LCD lcd((uint16_t *) &fbuf, ROT_180);

    lcd.fillScreen(LCD_BLACK);
    lcd.update();

    // Backlight on (if using PIN_BLK to gate of transistor)
    gpio_put(PIN_BLK, true);

    // Demo
    //lcd_demo(&lcd);
    lcd.drawCircle(LCD_W_CENTER, LCD_H_CENTER, 20, LCD_WHITE, 1, true);
    lcd.update();

//    //Radar objects
//    struct object o1 = {0};
//    struct object o2 = {0};
//    struct object o3 = {0};
//    struct object o4 = {0};
//    struct object o5 = {0};
//
//    // Idle loop
//    while (true) {
//
//        lcd.fillScreen(LCD_BLACK);
//
//        object_processing(&lcd,&o1);
//        object_processing(&lcd,&o2);
//        object_processing(&lcd,&o3);
//        object_processing(&lcd,&o4);
//        object_processing(&lcd,&o5);
//
//        lcd.drawLine(LCD_W_CENTER, 0, LCD_W_CENTER, GC9107_HEIGHT, LCD_DARKGREEN, 1);
//        lcd.drawLine(0, LCD_H_CENTER, GC9107_WIDTH, LCD_H_CENTER, LCD_DARKGREEN, 1);
//
//        lcd.drawCircle(LCD_W_CENTER, LCD_H_CENTER, 20, LCD_DARKGREEN, 1, true);
//        lcd.drawCircle(LCD_W_CENTER, LCD_H_CENTER, 40, LCD_DARKGREEN, 1, false);
//        lcd.drawCircle(LCD_W_CENTER, LCD_H_CENTER, 60, LCD_DARKGREEN, 1, false);
//        lcd.drawCircle(LCD_W_CENTER, LCD_H_CENTER, 100, LCD_DARKGREEN, 1, false);
//        lcd.drawText(45, LCD_HEIGHT/2, "SPS", &oswald_bold_12, LCD_BLACK, LCD_DARKGREEN);
//
//        lcd.update();
//    }
    //lcd.draw_gImage( 0,0, gImage_ava);

    //Halloween
    while(1){
        lcd.draw_gImage( 0,0, gImage_pumpkin_1);
        lcd.update();
        sleep_ms(get_rand_32()%200);
        lcd.draw_gImage( 0,0, gImage_pumpkin_2uo);
        lcd.update();
        sleep_ms(get_rand_32()%200);
    }
}