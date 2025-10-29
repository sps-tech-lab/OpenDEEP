#include "lcd.hpp"
#include <stdio.h>
#include "graphics.hpp"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "platform.hpp"
#include "GC9107.hpp"
#include "bsp.hpp"


LCD::LCD(uint16_t* canvas): FrameBuffer(canvas, LCD_HEIGHT, LCD_WIDTH)
{

}

LCD::LCD(uint16_t* canvas, uint8_t direction): FrameBuffer(canvas, LCD_HEIGHT, LCD_WIDTH)
{

}

void LCD::reset()
{

}

void LCD::command(uint8_t _command)
{
    lcd_cmd(&lcd, _command);
}

void LCD::data(uint8_t _data)
{
}

void LCD::set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    gc9107_set_addr_window(&lcd, x0, y0, (x1-x0), (y1-y0));
}


void LCD::init_reg()
{

}


void LCD::set_direction(uint8_t _direction){

    this->direction = _direction;

    uint8_t MemoryAccessReg=0x08;

    switch(_direction){
        case 0: MemoryAccessReg=0x08;break;
        case 1: MemoryAccessReg=0x68;break;
        case 2: MemoryAccessReg=0xc8;break;
        case 3: MemoryAccessReg=0xa8;break;
    }

    this->command(0x36);            //MX, MY, RGB mode
    this->data(MemoryAccessReg);    //0x08 set RGB
}


uint8_t LCD::get_direction(){
    return this->direction;
}

void LCD::update()
{
    // Define a full-screen window
    gc9107_set_addr_window(&lcd, 0,0, GC9107_WIDTH, GC9107_HEIGHT);

    lcd.gpio_dc(true);
    lcd.gpio_cs(false);

    // uint32_t count = (uint32_t)w * h;
    // for (uint32_t i = 0; i < count; ++i) {
    //     lcd.spi_tx(this->canvas, 2);
    // }
    lcd.gpio_cs(true);
}
