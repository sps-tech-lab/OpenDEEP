//
// Created by :: SPS :: on 29/08/2026.
//
// @Brief: Virtual GC9107 display controller
//
#include "sim_panel.hpp"
#include <cstring>
#include "GC9107.hpp"
#include "bsp.hpp"
#include "sim_window.hpp"

namespace {

// MADCTL bits (GC9107 / ILI-family layout).
constexpr uint8_t kMadctlMy = 0x80;  // row address order
constexpr uint8_t kMadctlMx = 0x40;  // column address order
constexpr uint8_t kMadctlMv = 0x20;  // row/column exchange
constexpr uint8_t kMadctlBgr = 0x08; // RGB/BGR order

constexpr size_t kGramPixels = static_cast<size_t>(SIM_GRAM_WIDTH) * SIM_GRAM_HEIGHT;

struct PanelState {
    uint16_t gram[kGramPixels] = {0};

    // Bus framing
    bool cs_asserted = false; // CS is active-low(true=selected)
    bool data_mode = false;   // DC high -> parameter/pixel data

    // Command decoding
    uint8_t command = 0;
    uint8_t params[8] = {0};
    uint8_t param_count = 0;

    // Address window and write cursor
    uint16_t x0 = 0;
    uint16_t x1 = SIM_GRAM_WIDTH - 1;
    uint16_t y0 = 0;
    uint16_t y1 = SIM_GRAM_HEIGHT - 1;
    uint32_t cursor = 0; // pixel index within active window
    bool frame_pending = false;

    // Mode
    uint8_t madctl = 0;
    bool inversion = false;
    bool display_on = false;
    bool backlight = false;
};

PanelState g_panel;

// Number of pixels the active RAMWR window holds.
uint32_t window_pixels() {
    const uint32_t w = static_cast<uint32_t>(g_panel.x1 - g_panel.x0) + 1u;
    const uint32_t h = static_cast<uint32_t>(g_panel.y1 - g_panel.y0) + 1u;
    return w * h;
}

// Map the n-th pixel of the active window onto frame-memory coordinates
// applying MADCTL address-counter direction exactly as controller
void cursor_to_gram(uint32_t n, uint16_t& gx, uint16_t& gy) {
    const uint32_t w = static_cast<uint32_t>(g_panel.x1 - g_panel.x0) + 1u;
    const uint32_t h = static_cast<uint32_t>(g_panel.y1 - g_panel.y0) + 1u;

    uint32_t dx = 0;
    uint32_t dy = 0;
    if( g_panel.madctl & kMadctlMv ) {
        // Row/column exchange: the counter walks down a column first
        dx = n / h;
        dy = n % h;
    } else {
        dx = n % w;
        dy = n / w;
    }

    gx = (g_panel.madctl & kMadctlMx) ? static_cast<uint16_t>(g_panel.x1 - dx) : static_cast<uint16_t>(g_panel.x0 + dx);
    gy = (g_panel.madctl & kMadctlMy) ? static_cast<uint16_t>(g_panel.y1 - dy) : static_cast<uint16_t>(g_panel.y0 + dy);
}

void write_pixel(uint16_t colour) {
    const uint32_t total = window_pixels();
    if( total == 0 ) {
        return;
    }

    uint16_t gx = 0;
    uint16_t gy = 0;
    cursor_to_gram(g_panel.cursor % total, gx, gy);

    if( gx < SIM_GRAM_WIDTH && gy < SIM_GRAM_HEIGHT ) {
        g_panel.gram[static_cast<size_t>(gy) * SIM_GRAM_WIDTH + gx] = colour;
    }

    g_panel.cursor++;
    if( g_panel.cursor >= total ) {
        g_panel.frame_pending = true;
    }
}

uint16_t swap_rb(uint16_t colour) {
    const uint16_t r = static_cast<uint16_t>((colour >> 11) & 0x1F);
    const uint16_t g = static_cast<uint16_t>((colour >> 5) & 0x3F);
    const uint16_t b = static_cast<uint16_t>(colour & 0x1F);
    return static_cast<uint16_t>((b << 11) | (g << 5) | r);
}

// Copy the visible slice of frame memory to the window
void present_frame() {
    static uint16_t visible[static_cast<size_t>(GC9107_WIDTH) * GC9107_HEIGHT];

    const bool invert = (g_panel.inversion != (SIM_PANEL_INVERTED != 0));
    const bool bgr = (((g_panel.madctl & kMadctlBgr) != 0) != (SIM_PANEL_BGR_WIRED != 0));

    for( uint32_t y = 0; y < GC9107_HEIGHT; y++ ) {
        for( uint32_t x = 0; x < GC9107_WIDTH; x++ ) {
            uint32_t sx = SIM_PANEL_X_ORIGIN + x;
            uint32_t sy = SIM_PANEL_Y_ORIGIN + y;

#if SIM_PANEL_MOUNT_ROTATION == 180
            sx = SIM_PANEL_X_ORIGIN + (GC9107_WIDTH - 1 - x);
            sy = SIM_PANEL_Y_ORIGIN + (GC9107_HEIGHT - 1 - y);
#endif

            uint16_t colour = g_panel.display_on ? g_panel.gram[sy * SIM_GRAM_WIDTH + sx] : 0;
            if( invert ) {
                colour = static_cast<uint16_t>(~colour);
            }
            if( bgr ) {
                colour = swap_rb(colour);
            }
            visible[static_cast<size_t>(y) * GC9107_WIDTH + x] = colour;
        }
    }

    sim_window_present(visible, GC9107_WIDTH, GC9107_HEIGHT, g_panel.backlight);
}

void dispatch_command() {
    switch( g_panel.command ) {
    case GC9107_CASET:
        if( g_panel.param_count >= 4 ) {
            g_panel.x0 = static_cast<uint16_t>((g_panel.params[0] << 8) | g_panel.params[1]);
            g_panel.x1 = static_cast<uint16_t>((g_panel.params[2] << 8) | g_panel.params[3]);
            g_panel.param_count = 0;
        }
        break;

    case GC9107_RASET:
        if( g_panel.param_count >= 4 ) {
            g_panel.y0 = static_cast<uint16_t>((g_panel.params[0] << 8) | g_panel.params[1]);
            g_panel.y1 = static_cast<uint16_t>((g_panel.params[2] << 8) | g_panel.params[3]);
            g_panel.param_count = 0;
        }
        break;

    case GC9107_MADCTL:
        if( g_panel.param_count >= 1 ) {
            g_panel.madctl = g_panel.params[0];
            g_panel.param_count = 0;
        }
        break;

    default:
        break;
    }
}

void apply_immediate_command(uint8_t command) {
    switch( command ) {
    case GC9107_SWRESET:
        std::memset(g_panel.gram, 0, sizeof(g_panel.gram));
        g_panel.madctl = 0;
        g_panel.inversion = false;
        g_panel.display_on = false;
        break;
    case GC9107_INVON:
        g_panel.inversion = true;
        break;
    case GC9107_INVOFF:
        g_panel.inversion = false;
        break;
    case GC9107_DISPON:
        g_panel.display_on = true;
        break;
    case GC9107_DISPOFF:
        g_panel.display_on = false;
        break;
    case GC9107_RAMWR:
        g_panel.cursor = 0;
        g_panel.frame_pending = false;
        break;
    default:
        break;
    }
}

} // namespace

void sim_panel_set_cs(bool level) {
    g_panel.cs_asserted = !level; // active low
}

void sim_panel_set_dc(bool level) {
    g_panel.data_mode = level;
}

void sim_panel_set_reset(bool level) {
    if( !level ) {
        // Hold in reset
        g_panel.display_on = false;
    }
}

void sim_panel_set_backlight(bool on) {
    g_panel.backlight = on;
}

void sim_panel_write8(const uint8_t* data, size_t len) {
    if( data == nullptr ) {
        return;
    }

    for( size_t i = 0; i < len; i++ ) {
        const uint8_t byte = data[i];

        if( !g_panel.data_mode ) {
            g_panel.command = byte;
            g_panel.param_count = 0;
            apply_immediate_command(byte);
            continue;
        }

        if( g_panel.command == GC9107_RAMWR ) {
            // Pixel data arriving over an 8-bit transfer: high byte then low byte.
            g_panel.params[g_panel.param_count++] = byte;
            if( g_panel.param_count >= 2 ) {
                write_pixel(static_cast<uint16_t>((g_panel.params[0] << 8) | g_panel.params[1]));
                g_panel.param_count = 0;
            }
            continue;
        }

        if( g_panel.param_count < sizeof(g_panel.params) ) {
            g_panel.params[g_panel.param_count++] = byte;
        }
        dispatch_command();
    }
}

void sim_panel_write16(const uint16_t* data, size_t len) {
    if( data == nullptr || !g_panel.data_mode ) {
        return;
    }

    for( size_t i = 0; i < len; i++ ) {
        write_pixel(data[i]);
    }

    if( g_panel.frame_pending ) {
        g_panel.frame_pending = false;
        present_frame();
    }
}
