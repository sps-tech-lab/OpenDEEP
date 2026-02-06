#include "graphics.hpp"
#include <stdint.h>
#include <stdlib.h>
#include <cstdarg>
#include <cstdio>

FrameBuffer::FrameBuffer(uint16_t* _canvas, uint8_t _width, uint8_t _height)
{
    this->canvas = _canvas;
    this->width  = _width;
    this->height = _height;
}

void FrameBuffer::setPixel(uint8_t x, uint8_t y, uint16_t color)
{
    if( x >= this->width || y >= this->height ) return;

    this->canvas[x + y*this->width] = color;
}


void FrameBuffer::drawPoint(uint8_t x, uint8_t y, uint16_t color, uint8_t size)
{
    if( x >= this->width || y >= this->height ) return;

        for (int16_t x_i = 0; x_i < size; x_i++) {
            for (int16_t y_i = 0; y_i < size; y_i++) {
                this->setPixel(x + x_i, y + y_i, color);
            }
        }
}

void FrameBuffer::drawLine(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint16_t color, uint8_t size)
{

    if(x_start > this->width || y_start > this->height || x_end > this->width || y_end > this->height) 
        return;
    
    uint8_t x_point = x_start;
    uint8_t y_point = y_start;

    int dx = (int)x_end - (int)x_start >= 0 ? x_end - x_start : x_start - x_end;
    int dy = (int)y_end - (int)y_start <= 0 ? y_end - y_start : y_start - y_end;

    // Increment direction, 1 is positive, -1 is counter;
    int XAddway = x_start < x_end ? 1 : -1;
    int YAddway = y_start < y_end ? 1 : -1;

    //Cumulative error
    int eps = dx + dy;

    for (;;)
    {

        this->drawPoint(x_point, y_point, color, size);

        if(2 * eps >= dy) 
        {
            if(x_point == x_end)
                break;
            eps += dy;
            x_point += XAddway;
        }

        if(2 * eps <= dx) 
        {
            if(y_point == y_end)
                break;

            eps += dx;
            y_point += YAddway;
        }

    }
}

void FrameBuffer::drawHorizontalLine(uint8_t x_start, uint8_t y_start, uint8_t line_width, uint16_t color, uint8_t size)
{
    if(x_start > this->width || y_start > this->height) {
        return;
    }
    
    for (uint8_t x_point = x_start; x_point < x_start + line_width; x_point++) {
        this->drawPoint(x_point, y_start, color, size);
    }
}

void FrameBuffer::drawVerticalLine(uint8_t x_start, uint8_t y_start, uint8_t line_height, uint16_t color, uint8_t size)
{
    if(x_start > this->width || y_start > this->height) {
        return;
    }
    
    for (uint8_t y_point = y_start; y_point < y_start + line_height; y_point++) {
        this->drawPoint(x_start, y_point, color, size);
    }
}


void FrameBuffer::drawRect(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint16_t color, uint8_t size, bool fill)
{
    if(x_start > this->width || y_start > this->height || x_end > this->width || y_end > this->height) {
        return;
    }

    if(fill) {
        for(uint16_t i = y_start; i < y_end; i++) {
            this->drawLine(x_start, i, x_end, i, color, size);
        }
    } else {
        this->drawLine(x_start, y_start, x_end, y_start, color, size);
        this->drawLine(x_start, y_start, x_start, y_end, color, size);
        this->drawLine(x_end, y_end, x_end, y_start, color, size);
        this->drawLine(x_end, y_end, x_start, y_end, color, size);
    }
}

void FrameBuffer::drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint16_t color, uint8_t size, bool fill)
{
    if( x >= this->width || y >= this->height ) return;

    int a, b;
    int di;

    if(fill) {
        di = 3 - (radius << 1);
        a = 0;
        b = radius;
        while (a <= b) {
            this->drawHorizontalLine(x - b, y - a, 2 * b + 1, color, size);
            this->drawHorizontalLine(x - b, y + a, 2 * b + 1, color, size);
            this->drawHorizontalLine(x - a, y - b, 2 * a + 1, color, size);
            this->drawHorizontalLine(x - a, y + b, 2 * a + 1, color, size);
            if(di < 0)
                di += 4 * a + 6;
            else {
                di += 10 + 4 * (a - b);
                b--;
            }
            a++;
        }
    } else {
        a = 0;
        b = radius;
        di = 3 - (radius << 1);
        while (a <= b) {
            this->drawPoint(x + a, y - b, color, size);
            this->drawPoint(x + b, y - a, color, size);
            this->drawPoint(x + b, y + a, color, size);
            this->drawPoint(x + a, y + b, color, size);
            this->drawPoint(x - a, y + b, color, size);
            this->drawPoint(x - b, y + a, color, size);
            this->drawPoint(x - b, y - a, color, size);
            this->drawPoint(x - a, y - b, color, size);
            if(di < 0)
                di += 4 * a + 6;
            else {
                di += 10 + 4 * (a - b);
                b--;
            }
            a++;
        }
    }
}


void FrameBuffer::fillScreen(uint16_t color)
{
    for (int i = 0; i < this->height*this->width; i++) {
        this->canvas[i] = color;
    }
}

void FrameBuffer::drawChar(uint8_t x, uint8_t y, const char ascii, font* font, uint16_t foreground, uint16_t background)
{
    if (x >= this->width || y >= this->height) return;

    // Pointer to start of font table
    const uint8_t* glyph = font->table;
    char target = ascii;

    // Skip through previous glyphs (each has [w, h, data...])
    for (uint8_t c = 32; c < target; c++) {
        uint8_t w = *glyph++;
        uint8_t h = *glyph++;
        uint8_t bytes_per_row = (w + 7) / 8;
        glyph += bytes_per_row * h;
    }

    // Read glyph metadata
    uint8_t glyph_width = *glyph++;
    uint8_t glyph_height = *glyph++;
    uint8_t bytes_per_row = (glyph_width + 7) / 8;

    for (uint8_t row = 0; row < glyph_height; row++) {
        for (uint8_t col = 0; col < glyph_width; col++) {
            uint8_t byte = glyph[col / 8];
            bool pixel_on = byte & (0x80 >> (col % 8));
            this->setPixel(x + col, y + row, pixel_on ? background : foreground);
        }
        glyph += bytes_per_row; // Advance to next row
    }
}

uint32_t FrameBuffer::drawText(uint8_t x, uint8_t y, const char *str, font *_font, uint16_t foreground, uint16_t background)
{
    return drawText(x, y, str, _font, foreground, background, 0);
}

uint32_t FrameBuffer::drawText(uint8_t x, uint8_t y, const char *str, font *_font, uint16_t foreground, uint16_t background, uint8_t spacing)
{
    uint8_t x_point = x;
    uint8_t y_point = y;

    if (x >= this->width || y >= this->height) return 0;

    while (*str != '\0')
    {
        // Find glyph start to read width/height
        const uint8_t* glyph = _font->table;
        for (uint8_t c = 32; c < *str; c++) {
            uint8_t w = *glyph++;
            uint8_t h = *glyph++;
            uint8_t bytes_per_row = (w + 7) / 8;
            glyph += bytes_per_row * h;
        }
        uint8_t char_width = *glyph++;
        uint8_t char_height = *glyph++;

        // Wrap handling
        if ((x_point + char_width) > this->width) {
            x_point = x;
            y_point += char_height;
        }

        if ((y_point + char_height) > this->height) {
            x_point = x;
            y_point = y;
        }

        this->drawChar(x_point, y_point, *str, _font, background, foreground);

        // Move cursor forward by actual glyph width
        x_point += char_width + spacing;
        str++;
    }
    return x_point;
}

uint32_t FrameBuffer::drawText(uint8_t x, uint8_t y, font *_font, uint16_t foreground, uint16_t background, uint8_t spacing, const char* fmt, ...)
{
    char buf[32];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    return drawText(x, y, buf, _font, foreground, background, spacing);
}

void FrameBuffer::darwGradientRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color1, uint16_t color2, bool direction)
{
    if(direction)
    {
        float delta = -255.0/h;
        float alpha = 255.0;
        uint16_t color = color1;

        while (h--) 
        {
            drawHorizontalLine(x, y++, w, color, 1);
            alpha += delta;
            color = alphaBlend((uint8_t)alpha, color1, color2);
        }
    }
    else
    {
        float delta = -255.0/w;
        float alpha = 255.0;
        uint16_t color = color1;

        while (w--) 
        {
            drawVerticalLine(x++, y, h, color, 1);
            alpha += delta;
            color = alphaBlend((uint8_t)alpha, color1, color2);
        }
    }
}

uint16_t FrameBuffer::alphaBlend(uint8_t alpha, uint16_t color1, uint16_t color2)
{
    // Split out and blend 5 bit red and blue channels
    uint32_t rxb = color2 & 0xF81F;
    rxb += ((color1 & 0xF81F) - rxb) * (alpha >> 2) >> 6;
    // Split out and blend 6 bit green channel
    uint32_t xgx = color2 & 0x07E0;
    xgx += ((color1 & 0x07E0) - xgx) * alpha >> 8;
    // Recombine channels
    return (rxb & 0xF81F) | (xgx & 0x07E0);
}

void FrameBuffer::drawBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t* data)
{
    for(uint8_t row = 0; row < h; ++row) {
        for(uint8_t col = 0; col < w; ++col) {
            setPixel(x + col, y + row,
                     data[row * w + col]);
        }
    }
}

void FrameBuffer::drawMonoBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* data, uint16_t fg, uint16_t bg)
{
    uint8_t bytesPerRow = (w + 7) / 8;
    for(uint8_t row = 0; row < h; ++row) {
        const uint8_t* ptr = data + row * bytesPerRow;
        for(uint8_t col = 0; col < w; ++col) {
            bool bit = ptr[col >> 3] & (0x80 >> (col & 7));
            setPixel(x + col, y + row,
                     bit ? fg : bg);
        }
    }
}

void FrameBuffer::draw_gImage(int x, int y, const unsigned char *data)
{
    //Parse that 8-byte head
    uint16_t w =  uint16_t(data[2])|(uint16_t(data[3]) << 8);
    uint16_t h =  uint16_t(data[4])|(uint16_t(data[5]) << 8);

    //Actual pixel data begins at data+8
    const unsigned char *pixels = data + 8;

    for (uint16_t row = 0; row < h; ++row) {
        for (uint16_t col = 0; col < w; ++col) {
            size_t idx = 2*(row * w + col);

            // little-endian
            uint16_t raw = (uint16_t(pixels[idx+1]) << 8)| uint16_t(pixels[idx]);
            setPixel(x + col, y + row, raw);
        }
    }
}