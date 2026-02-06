#include "lcd.hpp"
#include <stdio.h>
#include "graphics.hpp"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "platform.hpp"
#include "GC9107.hpp"
#include "bsp.hpp"


LCD::LCD(uint16_t* canvas): FrameBuffer(canvas, GC9107_WIDTH, GC9107_HEIGHT)
{
    this->reset();
    sleep_ms(100);
    this->init_reg();
    this->set_direction(HORIZONTAL);

    this->fillScreen(LCD_BLACK);
    this->update();
}

LCD::LCD(uint16_t* canvas, uint8_t direction): FrameBuffer(canvas, GC9107_WIDTH, GC9107_HEIGHT)
{
    this->reset();
    sleep_ms(100);
    this->init_reg();
    this->set_direction(direction);

    this->fillScreen(LCD_BLACK);
    this->update();
}

void LCD::reset()
{
    gpio_put(PIN_RST, true);
    sleep_ms(100);
    gpio_put(PIN_RST, false);
    sleep_ms(100);
    gpio_put(PIN_RST, true);
    gpio_put(PIN_CS,  false);
    sleep_ms(100);
}

void LCD::command(uint8_t _command)
{
    gpio_put(PIN_DC,  false);
    gpio_put(PIN_CS,  false);
    spi_write_blocking(SPI_INSTANCE(PICO_DEFAULT_SPI), &_command, 1);
    gpio_put(PIN_CS,  true);
}

void LCD::data(uint8_t _data)
{
    gpio_put(PIN_DC,  true);
    gpio_put(PIN_CS,  false);
    spi_write_blocking(SPI_INSTANCE(PICO_DEFAULT_SPI), &_data, 1);
    gpio_put(PIN_CS,  true);
}

void LCD::data_buf(const uint8_t* _buf, uint8_t _len)
{
    gpio_put(PIN_DC,  true);
    gpio_put(PIN_CS,  false);
    spi_write_blocking(SPI_INSTANCE(PICO_DEFAULT_SPI), _buf, _len);
    gpio_put(PIN_CS,  true);
}

void LCD::set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // Apply internal offsets
    x0 += GC9107.x_offset;
    x1 += GC9107.x_offset;
    y0 += 13;   //GC9107.y_offset;
    y1 += 13;   //GC9107.y_offset;

    // Build column and row address data
    uint8_t ca[4] = { (uint8_t)(x0 >> 8), (uint8_t)x0,(uint8_t)(x1 >> 8), (uint8_t)x1 };
    uint8_t ra[4] = { (uint8_t)(y0 >> 8), (uint8_t)y0, (uint8_t)(y1 >> 8), (uint8_t)y1 };

    this->command(GC9107_CASET);
    this->data_buf(ca, 4);
    this->command(GC9107_RASET);
    this->data_buf(ra, 4);
    this->command(GC9107_RAMWR);
}


void LCD::init_reg()
{
    // HW reset
    gpio_put(PIN_RST, false); sleep_ms(10);
    gpio_put(PIN_RST, true);  sleep_ms(120);

    // SW reset
    this->command(GC9107_SWRESET);
    sleep_ms(120);

    // Sleep out
    this->command(GC9107_SLPOUT);
    sleep_ms(120);

    // Pixel format: 16bpp (RGB565)
    this->command(GC9107_COLMOD);
    this->data(0x55);

    // Inversion ON
    this->command(GC9107_INVON);

    // Full window
    this->command(GC9107_CASET);
    this->data(0x00);
    this->data(0x00);
    this->data(0x00);
    this->data(GC9107_WIDTH  - 1);
    
    this->command(GC9107_RASET);
    this->data(0x00);
    this->data(0x00);
    this->data(0x00);
    this->data(GC9107_HEIGHT - 1);

    // Display on
    this->command(GC9107_DISPON);
    sleep_ms(50);

    gpio_put(PIN_BLK, true);
}


void LCD::set_direction(uint8_t _direction){
    this->direction = _direction;

    uint8_t MemoryAccessReg = _direction | 0x08; //BGR bit

    this->command(GC9107_MADCTL);
    this->data(MemoryAccessReg);
}


uint8_t LCD::get_direction(){
    return this->direction;
}

void LCD::update()
{
    // Define a full-screen window
    this->set_window(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);

    // Switch into 16-bit mode for bulk pixel data
    spi_set_format(SPI_INSTANCE(PICO_DEFAULT_SPI), 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_put(PIN_DC,  true);
    gpio_put(PIN_CS,  false);

    spi_write16_blocking(SPI_INSTANCE(PICO_DEFAULT_SPI), this->canvas, size_t(LCD_WIDTH) * LCD_HEIGHT);

    gpio_put(PIN_CS,  true);

    // Back to 8-bit
    spi_set_format(SPI_INSTANCE(PICO_DEFAULT_SPI),8, SPI_CPOL_0, SPI_CPHA_0,SPI_MSB_FIRST);
}
