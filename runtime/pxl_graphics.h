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
} PxlContext;

extern PxlContext pxl_context;

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
void loop(void);

void noLoop(void);

// --- Color / settings
void background(float r, float g, float b); // clears the canvas
void fill(float r, float g, float b);

void fill_a(float r, float g, float b, float a);

void noFill(void);

void stroke(float r, float g, float b);

void stroke_a(float r, float g, float b, float a);

void noStroke(void);

void strokeWeight(float weight);

void colorMode(PxlColorMode mode);

// --- 2D Primitives
void rect(float x, float y, float w, float h);

void circle(float x, float y, float d); // x,y = center; d = diameter
void ellipse(float x, float y, float w, float h); // x,y = center
void line(float x1, float y1, float x2, float y2);

void triangle(float x1, float y1, float x2, float y2, float x3, float y3);

void point(float x, float y);

// --- Transforms
void translate(float x, float y);

void rotate(float angle); // radians
void scale_xy(float sx, float sy);

void push(void);

void pop(void);

// --- Math
float map_val(float value, float start1, float stop1, float start2, float stop2);

float lerp_val(float start, float stop, float amt);

float constrain_val(float n, float low, float high);

float dist_val(float x1, float y1, float x2, float y2);

float random_val(float low, float high);

float noise_val(float x); // 1D Perlin-style value noise
float noise_val2(float x, float y); // 2D

// ============================================================
//  Input accessors  (read each frame inside draw())
// ============================================================
//  Access pxl_context.mouseX / mouseY / mouse_pressed / key_pressed / key_code
//  directly, or use these thin macros for p5.js naming:
// ============================================================
#define MOUSE_X        (pxl_context.mouseX)
#define MOUSE_Y        (pxl_context.mouseY)
#define MOUSE_PRESSED  (pxl_context.mouse_pressed)
#define KEY_PRESSED    (pxl_context.key_pressed)
#define KEY_CODE       (pxl_context.key_code)

#endif // COMPILER_PROJECT_PXL_GRAPHICS_H
