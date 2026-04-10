#ifndef COMPILER_PROJECT_PXL_GRAPHICS_H
#define COMPILER_PROJECT_PXL_GRAPHICS_H

#include <SDL3/SDL_render.h>
#include <stdbool.h>

// ============================================================
//  Color modes
// ============================================================
typedef enum {
    PXL_COLOR_MODE_RGB = 0,
    PXL_COLOR_MODE_HSB = 1
} PxlColorMode;

// ============================================================
//  Text alignment (mirrors p5.js constants)
// ============================================================
typedef enum {
    PXL_ALIGN_LEFT = 0,
    PXL_ALIGN_CENTER = 1,
    PXL_ALIGN_RIGHT = 2
} PxlTextAlign;

// ============================================================
//  2×3 affine transform matrix (column-major, no perspective)
//  [ a  c  tx ]
//  [ b  d  ty ]
// ============================================================
typedef struct {
    float a, b, c, d; // rotation / scale
    float tx, ty; // translation
} PxlMatrix;

#define PXL_MAX_TRANSFORM_STACK 64

// ============================================================
//  Global context
// ============================================================
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;

    int width;
    int height;
    Uint32 fps;
    bool looping;
    const char* window_title;

    // p5.js-style color state
    SDL_Color fill_color;
    SDL_Color stroke_color;
    SDL_Color background_color;
    bool has_fill;
    bool has_stroke;
    float stroke_weight;
    PxlColorMode color_mode;

    // Text
    float text_size;
    PxlTextAlign text_align;

    // Transform stack
    PxlMatrix transform_stack[PXL_MAX_TRANSFORM_STACK];
    int transform_stack_top; // -1 = empty (identity in use)
    PxlMatrix current_transform;

    // Input state (updated each frame by pxl_run)
    float mouseX;
    float mouseY;
    bool mouse_pressed;
    SDL_Keycode key_code; // last key pressed this frame
    bool key_pressed; // true for one frame when a key goes down

    SDL_Texture* canvas;
} PxlContext;

extern float pxl_mouse_x;
extern float pxl_mouse_y;
extern bool pxl_mouse_pressed;
extern bool pxl_key_pressed;
extern int pxl_key_code;

// ============================================================
//  Lifecycle  (called by pxl_run — users rarely touch these)
// ============================================================
void pxl_init(void);

void pxl_run(void (*setup_fn)(void), void (*draw_fn)(void));

void pxl_quit(void);

// ============================================================
//  Pre-setup configuration  (call before setup() / pxl_run())
// ============================================================
void pxl_set_canvas_size(int width, int height);

void pxl_set_frames_per_second(Uint32 fps);

void pxl_set_window_title(const char* title);

// ============================================================
//  p5.js-style API  (call from setup() / draw())
// ============================================================

// --- Structure
void pxl_loop(void);

void pxl_no_loop(void);

// --- Color / settings
void pxl_background(float r, float g, float b); // clears the canvas
void pxl_fill(float r, float g, float b);

void pxl_fill_a(float r, float g, float b, float a);

void pxl_no_fill(void);

void pxl_stroke(float r, float g, float b);

void pxl_stroke_a(float r, float g, float b, float a);

void pxl_no_stroke(void);

void pxl_stroke_weight(float weight);

void pxl_color_mode(PxlColorMode mode);

// --- 2D Primitives
void pxl_rect(float x, float y, float w, float h);

void pxl_circle(float x, float y, float d); // x,y = center; d = diameter
void pxl_ellipse(float x, float y, float w, float h); // x,y = center
void pxl_line(float x1, float y1, float x2, float y2);

void pxl_triangle(float x1, float y1, float x2, float y2, float x3, float y3);

void pxl_point(float x, float y);

// --- Transforms
void pxl_translate(float x, float y);

void pxl_rotate(float angle); // radians
void pxl_scale(float sx, float sy);

void pxl_push(void);

void pxl_pop(void);

// --- Math
float pxl_map(float value, float start1, float stop1, float start2, float stop2);

float pxl_lerp(float start, float stop, float amt);

float pxl_constrain(float n, float low, float high);

float pxl_dist(float x1, float y1, float x2, float y2);

float pxl_random(float low, float high);

float pxl_noise(float x); // 1D Perlin-style value noise
float pxl_noise2(float x, float y); // 2D

#endif // COMPILER_PROJECT_PXL_GRAPHICS_H
