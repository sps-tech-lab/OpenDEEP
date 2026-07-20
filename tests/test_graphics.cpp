//
// Created by SPS on 20/07/2026.
//
// FrameBuffer graphics unit-tests
//

// Includes
#include <array>
#include <cstdint>
#include <vector>
#include <catch2/catch_test_macros.hpp>
#include "graphics.hpp"

namespace {

struct TestCanvas : FrameBuffer {
    std::vector<uint16_t> pixels;

    TestCanvas(uint8_t w, uint8_t h) : FrameBuffer(nullptr, w, h), pixels(static_cast<size_t>(w) * h, 0) {
        this->canvas = pixels.data();
    }

    void update() override {} // no panel to flush to on the host

    uint16_t at(uint8_t x, uint8_t y) const {
        return pixels[static_cast<size_t>(x) + static_cast<size_t>(y) * width];
    }

    size_t count(uint16_t color) const {
        size_t n = 0;
        for( uint16_t p : pixels ) {
            if( p == color ) {
                ++n;
            }
        }
        return n;
    }
};

constexpr uint16_t kFg = 0xABCD;
constexpr uint16_t kBg = 0x1234;

// A synthetic 2-glyph font so drawChar/drawText
// Layout per glyph: [w][h][packed rows, MSB first].
// Glyph ASCII 32 ' ' and ASCII 33 '!' are both 8x2
const uint8_t kFontTable[] = {
    8, 2, 0xFF, 0x00, // ' '
    8, 2, 0x81, 0x18, // '!'
};

font make_font() {
    return font(kFontTable, 8, 2);
}

} // namespace

// ---------------------------------------------------------------------------
// Color packing
// ---------------------------------------------------------------------------

TEST_CASE("RGB565 packs the primaries into the expected 16-bit words") {
    CHECK(RGB565(0, 0, 0) == 0x0000);
    CHECK(RGB565(255, 255, 255) == 0xFFFF);
    CHECK(RGB565(255, 0, 0) == 0xF800); // 5 red bits, top
    CHECK(RGB565(0, 255, 0) == 0x07E0); // 6 green bits, middle
    CHECK(RGB565(0, 0, 255) == 0x001F); // 5 blue bits, bottom
}

TEST_CASE("RGB565 truncates the low channel bits that do not fit") {
    CHECK(RGB565(7, 3, 7) == 0x0000);
    CHECK(RGB565(0xF8, 0xFC, 0xF8) == RGB565(0xFF, 0xFF, 0xFF));
}

// ---------------------------------------------------------------------------
// setPixel / fillScreen
// ---------------------------------------------------------------------------

TEST_CASE("setPixel writes at x + y*width and clips out-of-range coordinates") {
    TestCanvas fb(10, 8);

    fb.setPixel(3, 2, kFg);
    CHECK(fb.at(3, 2) == kFg);
    CHECK(fb.pixels[3 + 2 * 10] == kFg);

    SECTION("x == width and y == height are rejected (upper bound is exclusive)") {
        fb.setPixel(10, 0, kFg);
        fb.setPixel(0, 8, kFg);
        CHECK(fb.count(kFg) == 1); // still only the first write survived
    }
}

TEST_CASE("fillScreen paints every pixel") {
    TestCanvas fb(10, 8);
    fb.fillScreen(kFg);
    CHECK(fb.count(kFg) == 10u * 8u);
}

// ---------------------------------------------------------------------------
// drawPoint
// ---------------------------------------------------------------------------

TEST_CASE("drawPoint of size 1 sets a single pixel") {
    TestCanvas fb(10, 10);
    fb.drawPoint(4, 4, kFg, 1);
    CHECK(fb.count(kFg) == 1);
    CHECK(fb.at(4, 4) == kFg);
}

TEST_CASE("drawPoint of size N fills an NxN block and clips at the edge") {
    TestCanvas fb(10, 10);
    fb.drawPoint(2, 2, kFg, 3);
    CHECK(fb.count(kFg) == 9);
    for( uint8_t y = 2; y < 5; ++y ) {
        for( uint8_t x = 2; x < 5; ++x ) {
            CHECK(fb.at(x, y) == kFg);
        }
    }

    SECTION("a block straddling the right/bottom edge only writes visible pixels") {
        TestCanvas edge(10, 10);
        edge.drawPoint(9, 9, kFg, 3); // only (9,9) is on-screen
        CHECK(edge.count(kFg) == 1);
        CHECK(edge.at(9, 9) == kFg);
    }
}

// ---------------------------------------------------------------------------
// Horizontal / vertical lines
// ---------------------------------------------------------------------------

TEST_CASE("drawHorizontalLine spans the requested width on one row") {
    TestCanvas fb(20, 10);
    fb.drawHorizontalLine(2, 5, 6, kFg, 1);
    for( uint8_t x = 2; x < 8; ++x ) {
        CHECK(fb.at(x, 5) == kFg);
    }
    CHECK(fb.count(kFg) == 6);
}

TEST_CASE("drawVerticalLine spans the requested height on one column") {
    TestCanvas fb(20, 10);
    fb.drawVerticalLine(4, 1, 7, kFg, 1);
    for( uint8_t y = 1; y < 8; ++y ) {
        CHECK(fb.at(4, y) == kFg);
    }
    CHECK(fb.count(kFg) == 7);
}

// ---------------------------------------------------------------------------
// drawLine (Bresenham)
// ---------------------------------------------------------------------------

TEST_CASE("drawLine draws a horizontal run between the endpoints") {
    TestCanvas fb(20, 10);
    fb.drawLine(2, 3, 8, 3, kFg, 1);
    for( uint8_t x = 2; x <= 8; ++x ) {
        CHECK(fb.at(x, 3) == kFg);
    }
}

TEST_CASE("drawLine draws a vertical run between the endpoints") {
    TestCanvas fb(20, 10);
    fb.drawLine(6, 1, 6, 7, kFg, 1);
    for( uint8_t y = 1; y <= 7; ++y ) {
        CHECK(fb.at(6, y) == kFg);
    }
}

TEST_CASE("drawLine hits both endpoints of a 45-degree diagonal") {
    TestCanvas fb(20, 20);
    fb.drawLine(0, 0, 5, 5, kFg, 1);
    CHECK(fb.at(0, 0) == kFg);
    CHECK(fb.at(5, 5) == kFg);
    // the diagonal of a 6-pixel span touches 6 cells
    CHECK(fb.count(kFg) == 6);
    for( uint8_t i = 0; i <= 5; ++i ) {
        CHECK(fb.at(i, i) == kFg);
    }
}

// ---------------------------------------------------------------------------
// drawRect
// ---------------------------------------------------------------------------

TEST_CASE("drawRect outline draws the four edges and leaves the interior clear") {
    TestCanvas fb(20, 20);
    fb.drawRect(2, 2, 10, 8, kFg, 1, /*fill=*/false);

    CHECK(fb.at(2, 2) == kFg);   // corners
    CHECK(fb.at(10, 2) == kFg);
    CHECK(fb.at(2, 8) == kFg);
    CHECK(fb.at(10, 8) == kFg);
    CHECK(fb.at(6, 5) == 0);     // interior untouched
}

TEST_CASE("drawRect fill paints the interior span") {
    TestCanvas fb(20, 20);
    fb.drawRect(2, 2, 10, 8, kFg, 1, /*fill=*/true);
    CHECK(fb.at(6, 5) == kFg);
    CHECK(fb.count(kFg) > 0);
}

// ---------------------------------------------------------------------------
// drawCircle
// ---------------------------------------------------------------------------

TEST_CASE("drawCircle outline is symmetric and roughly at radius distance") {
    TestCanvas fb(40, 40);
    const uint8_t cx = 20;
    const uint8_t cy = 20;
    const uint8_t r = 8;
    fb.drawCircle(cx, cy, r, kFg, 1, /*fill=*/false);

    // cardinal points of the circle must be lit
    CHECK(fb.at(cx + r, cy) == kFg);
    CHECK(fb.at(cx - r, cy) == kFg);
    CHECK(fb.at(cx, cy + r) == kFg);
    CHECK(fb.at(cx, cy - r) == kFg);

    // centre stays clear for an outline
    CHECK(fb.at(cx, cy) == 0);
}

TEST_CASE("drawCircle fill covers the centre") {
    TestCanvas fb(40, 40);
    fb.drawCircle(20, 20, 8, kFg, 1, /*fill=*/true);
    CHECK(fb.at(20, 20) == kFg);
    CHECK(fb.at(20 + 8, 20) == kFg);
}

// ---------------------------------------------------------------------------
// alphaBlend
// ---------------------------------------------------------------------------

TEST_CASE("alphaBlend returns the endpoints at the extreme alpha values") {
    TestCanvas fb(2, 2);

    SECTION("alpha 0 -> fully color2") {
        CHECK(fb.alphaBlend(0, LCD_WHITE, LCD_BLACK) == LCD_BLACK);
    }
    SECTION("alpha 255 -> essentially color1 (one quantisation step short)") {
        // alpha weights color1 by alpha/256 reaches 63/64 of the way to white, not the whole way
        uint16_t v = fb.alphaBlend(255, LCD_WHITE, LCD_BLACK);
        CHECK(((v >> 11) & 0x1F) >= 0x1F - 1);
        CHECK(((v >> 5) & 0x3F) >= 0x3F - 1);
        CHECK((v & 0x1F) >= 0x1F - 1);
    }
}

TEST_CASE("alphaBlend midpoint of black and white is a mid grey") {
    TestCanvas fb(2, 2);
    uint16_t mid = fb.alphaBlend(128, LCD_WHITE, LCD_BLACK);

    // decompose RGB565 & confirm every channel is between endpoints
    uint16_t r = (mid >> 11) & 0x1F;
    uint16_t g = (mid >> 5) & 0x3F;
    uint16_t b = mid & 0x1F;
    CHECK(r > 0);
    CHECK(r < 0x1F);
    CHECK(g > 0);
    CHECK(g < 0x3F);
    CHECK(b > 0);
    CHECK(b < 0x1F);
}

// ---------------------------------------------------------------------------
// Bitmaps
// ---------------------------------------------------------------------------

TEST_CASE("drawBitmap copies a full-color image row-major at the offset") {
    TestCanvas fb(10, 10);
    const std::array<uint16_t, 4> img = {0x1111, 0x2222, 0x3333, 0x4444}; // 2x2
    fb.drawBitmap(3, 4, 2, 2, img.data());

    CHECK(fb.at(3, 4) == 0x1111);
    CHECK(fb.at(4, 4) == 0x2222);
    CHECK(fb.at(3, 5) == 0x3333);
    CHECK(fb.at(4, 5) == 0x4444);
}

TEST_CASE("drawMonoBitmap unpacks bits MSB-first with per-row byte padding") {
    TestCanvas fb(20, 10);
    // 10px wide -> 2 bytes/row
    // Row0: 0b10000000 0b01000000 => cols 0 and 9 set
    const std::array<uint8_t, 2> row = {0x80, 0x40};
    fb.drawMonoBitmap(0, 0, 10, 1, row.data(), kFg, kBg);

    CHECK(fb.at(0, 0) == kFg);
    CHECK(fb.at(9, 0) == kFg);
    for( uint8_t x = 1; x < 9; ++x ) {
        CHECK(fb.at(x, 0) == kBg);
    }
}

TEST_CASE("draw_gImage reads the 8-byte header and little-endian RGB565 body") {
    TestCanvas fb(10, 10);

    // Header: bytes[2..3] = width, bytes[4..5] = height (little-endian)
    // A 2x1 image with pixels 0xBEEF then 0xDEAD (little-endian)
    const std::array<unsigned char, 8 + 4> data = {
        0x00, 0x00,       // bytes[0..1] ignored by the parser
        0x02, 0x00,       // width  = 2
        0x01, 0x00,       // height = 1
        0x00, 0x00,       // bytes[6..7] ignored
        0xEF, 0xBE,       // pixel 0 -> 0xBEEF
        0xAD, 0xDE,       // pixel 1 -> 0xDEAD
    };

    fb.draw_gImage(1, 1, data.data());

    CHECK(fb.at(1, 1) == 0xBEEF);
    CHECK(fb.at(2, 1) == 0xDEAD);
}

// ---------------------------------------------------------------------------
// Text
// ---------------------------------------------------------------------------

TEST_CASE("drawText navigates to a later glyph and renders it in the foreground") {
    TestCanvas fb(40, 10);
    font f = make_font();

    // set bits in the foreground color and clear bits in bg
    fb.drawText(0, 0, "!", &f, kFg, kBg);

    CHECK(fb.at(0, 0) == kFg); // set bit
    CHECK(fb.at(7, 0) == kFg); // set bit
    CHECK(fb.at(3, 0) == kBg); // clear bit -> background
}

TEST_CASE("drawText returns the advanced x cursor including spacing") {
    TestCanvas fb(60, 10);
    font f = make_font();

    // single 8px glyph starting at x=5 with spacing 2 -> 5+8+2
    uint32_t x_end = fb.drawText(5, 0, "!", &f, kFg, kBg, 2);
    CHECK(x_end == 5u + 8u + 2u);
}

TEST_CASE("drawText with a format string writes the formatted value") {
    TestCanvas fb(80, 10);
    font f = make_font();

    // "!!" -> two glyphs, cursor advances by 2*(8) with zero spacing
    uint32_t x_end = fb.drawText(0, 0, &f, kFg, kBg, 0, "%s", "!!");
    CHECK(x_end == 16u);
    CHECK(fb.at(0, 0) == kFg);  // first glyph
    CHECK(fb.at(8, 0) == kFg);  // second glyph
}
