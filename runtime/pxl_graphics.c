#include "pxl_graphics.h"

#include <stdlib.h>
#include <SDL3/SDL.h>

PxlContext pxl_context = {
    NULL, NULL,
    800, 600,
    60,
    {255, 255, 255, 255},
    {0, 0, 0, 255},
    "Pixel"
}; // Declare the global state of the graphics
// this library is not thread-safe and supports one window

void pxl_init() {
    if (pxl_context.width == 0 || pxl_context.height == 0) {
        SDL_Log("Canvas size not set before pxl_init\n");
        SDL_Quit();
    }
    atexit(pxl_quit);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL video: %s\n", SDL_GetError());
        SDL_Quit();
    }

    if (!SDL_CreateWindowAndRenderer(pxl_context.window_title, pxl_context.width, pxl_context.height, 0,
                                     &pxl_context.window,
                                     &pxl_context.renderer)) {
        SDL_Log("Failed to initialize window and renderer context: %s\n", SDL_GetError());
        SDL_Quit();
    }
}

void pxl_run(void (*setup_ptr)(void), void (*draw_ptr)(void)) {
    if (!setup_ptr || !draw_ptr) {
        SDL_Log("pxl_run: setup and draw callbacks must not be NULL\n");
        pxl_quit();
    }

    pxl_init();
    setup_ptr(); // call the user's setup()

    if (pxl_context.fps == 0) {
        SDL_Log("FPS must be greater than 0");
        return;
    }
    // Round up the delay to the closest integer
    const Uint64 target_frame_time_ms = (Uint64) (1000.0 / pxl_context.fps + 0.5);

    SDL_Event event;
    bool running = true;

    while (running) {
        const Uint64 start_time = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        draw_ptr(); // call the user's draw()

        if (!SDL_RenderPresent(pxl_context.renderer)) {
            SDL_Log("failed to render to screen: %s\n", SDL_GetError());
            pxl_quit();
            return;
        }

        const Uint64 frame_duration = SDL_GetTicks() - start_time;
        if (frame_duration < target_frame_time_ms) {
            SDL_Delay(target_frame_time_ms - frame_duration);
        }
    }
    pxl_quit();
}

void pxl_quit() {
    SDL_DestroyRenderer(pxl_context.renderer);
    SDL_DestroyWindow(pxl_context.window);
    SDL_Quit();
    exit(0);
}

void pxl_set_canvas_size(int width, int height) {
    pxl_context.width = width;
    pxl_context.height = height;
    if (pxl_context.window) {
        SDL_SetWindowSize(pxl_context.window, width, height);
    }
}

void pxl_set_frames_per_second(Uint32 fps) {
    pxl_context.fps = fps;
}

void pxl_set_background_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    pxl_context.background_color = (SDL_Color){r, g, b, a};
}

void pxl_set_draw_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    pxl_context.draw_color = (SDL_Color){r, g, b, a};
    if (pxl_context.renderer) {
        SDL_SetRenderDrawColor(pxl_context.renderer, r, g, b, a);
    }
}

void pxl_set_window_title(const char* title) {
    pxl_context.window_title = title;
    if (pxl_context.window) {
        SDL_SetWindowTitle(pxl_context.window, title);
    }
}

bool pxl_clear_screen() {
    pxl_set_draw_color(pxl_context.background_color.r, pxl_context.background_color.g, pxl_context.background_color.b,
                       pxl_context.background_color.a);

    if (!SDL_RenderClear(pxl_context.renderer)) {
        SDL_Log("failed to clear window: %s\n", SDL_GetError());
        return false;
    }

    return true;
}
