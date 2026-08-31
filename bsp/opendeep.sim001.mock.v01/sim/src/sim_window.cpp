//
// Created by :: SPS :: on 29/08/2026.
//
// @Brief: SDL2 simulator window
//

#define SDL_MAIN_HANDLED // app/main.cpp is the real entry point, not SDL_main

//Icludes
#include "sim_window.hpp"
#include <SDL.h>
#include <cstdio>
#include <vector>

namespace {

struct WindowState {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    int width = 0;
    int height = 0;

    std::vector<uint16_t> last_frame;
    int saved_frames = 0;

    float depth_bias = 0.0f;
    bool paused = false;
};

WindowState g_window;

[[noreturn]] void shutdown_and_exit() {
    if( g_window.texture != nullptr ) {
        SDL_DestroyTexture(g_window.texture);
    }
    if( g_window.renderer != nullptr ) {
        SDL_DestroyRenderer(g_window.renderer);
    }
    if( g_window.window != nullptr ) {
        SDL_DestroyWindow(g_window.window);
    }
    SDL_Quit();
    printf("[sim] window closed\n");
    std::exit(0);
}

void handle_key(const SDL_Keysym& key) {
    switch( key.sym ) {
    case SDLK_ESCAPE:
        shutdown_and_exit();
    case SDLK_UP:
        g_window.depth_bias += 0.1f;
        printf("[sim] depth bias %+.1f m\n", static_cast<double>(g_window.depth_bias));
        break;
    case SDLK_DOWN:
        g_window.depth_bias -= 0.1f;
        printf("[sim] depth bias %+.1f m\n", static_cast<double>(g_window.depth_bias));
        break;
    case SDLK_SPACE:
        g_window.paused = !g_window.paused;
        printf("[sim] dive profile %s\n", g_window.paused ? "paused" : "running");
        break;
    default:
        break;
    }
}

} // namespace

void sim_window_open(const char* title, int width, int height, int scale) {
    if( g_window.window != nullptr ) {
        return;
    }

    SDL_SetMainReady();
    if( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        printf("[sim] SDL_Init failed: %s\n", SDL_GetError());
        std::exit(1);
    }

    // No blur!
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    g_window.width = width;
    g_window.height = height;
    g_window.window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * scale,
                                       height * scale, SDL_WINDOW_SHOWN);
    if( g_window.window == nullptr ) {
        printf("[sim] SDL_CreateWindow failed: %s\n", SDL_GetError());
        std::exit(1);
    }

    g_window.renderer = SDL_CreateRenderer(g_window.window, -1, 0);
    if( g_window.renderer == nullptr ) {
        printf("[sim] SDL_CreateRenderer failed: %s\n", SDL_GetError());
        std::exit(1);
    }
    SDL_RenderSetLogicalSize(g_window.renderer, width, height);

    g_window.texture =
        SDL_CreateTexture(g_window.renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, width, height);
    if( g_window.texture == nullptr ) {
        printf("[sim] SDL_CreateTexture failed: %s\n", SDL_GetError());
        std::exit(1);
    }

    g_window.last_frame.assign(static_cast<size_t>(width) * height, 0);

    printf("[sim] window %dx%d at %dx scale\n", width, height, scale);
    printf("[sim] Up/Down nudge depth, Space pause, S save frame, Esc quit\n");
}

void sim_window_present(const uint16_t* rgb565, int width, int height, bool backlight) {
    if( g_window.texture == nullptr || rgb565 == nullptr ) {
        return;
    }

    const size_t pixels = static_cast<size_t>(width) * height;
    if( g_window.last_frame.size() == pixels ) {
        g_window.last_frame.assign(rgb565, rgb565 + pixels);
    }

    SDL_UpdateTexture(g_window.texture, nullptr, rgb565, width * static_cast<int>(sizeof(uint16_t)));

    // Imitate backlight off - real display is still readable in daylight
    const Uint8 level = backlight ? 255 : 40;

    SDL_SetTextureColorMod(g_window.texture, level, level, level);
    SDL_SetRenderDrawColor(g_window.renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_window.renderer);
    SDL_RenderCopy(g_window.renderer, g_window.texture, nullptr, nullptr);
    SDL_RenderPresent(g_window.renderer);

    sim_window_pump();
}

void sim_window_pump() {
    if( g_window.window == nullptr ) {
        return;
    }

    SDL_Event event;
    while( SDL_PollEvent(&event) != 0 ) {
        switch( event.type ) {
        case SDL_QUIT:
            shutdown_and_exit();
        case SDL_KEYDOWN:
            handle_key(event.key.keysym);
            break;
        default:
            break;
        }
    }
}

void sim_window_delay(uint32_t ms) {
    SDL_Delay(ms);
}

float sim_input_depth_bias() {
    return g_window.depth_bias;
}

bool sim_input_paused() {
    return g_window.paused;
}
