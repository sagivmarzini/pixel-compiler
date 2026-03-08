#ifndef COMPILER_PROJECT_PXL_GRAPHICS_H
#define COMPILER_PROJECT_PXL_GRAPHICS_H

#include <SDL3/SDL_render.h>
#include <stdbool.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;

    // Could later add more context data like mouse pos and is mouse pressed
    int width;
    int height;
    Uint32 fps;
    SDL_Color draw_color;
    SDL_Color background_color;
    const char* window_title;
} PxlContext;

extern PxlContext pxl_context; // declare global context

// === Lifecycle Functions
void pxl_init();

void pxl_run(void (*setup_ptr)(void), void (*draw_ptr)(void));

void pxl_quit();


// === Config setters
void pxl_set_canvas_size(int width, int height);

void pxl_set_frames_per_second(Uint32 fps);

void pxl_set_background_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

void pxl_set_draw_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

void pxl_set_window_title(const char* title);


bool pxl_clear_screen();

#endif //COMPILER_PROJECT_PXL_GRAPHICS_H
