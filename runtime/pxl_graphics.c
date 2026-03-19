#include "pxl_graphics.h"

#include <math.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

#include "pxl_error.h"

// ============================================================
//  Global context  —  p5.js defaults
// ============================================================
PxlContext pxl_context = {
    .window = NULL,
    .renderer = NULL,

    .width = 800,
    .height = 600,
    .fps = 60,
    .looping = true,
    .window_title = "Pixel",

    .fill_color = {255, 255, 255, 255}, // white fill
    .stroke_color = {0, 0, 0, 255}, // black stroke
    .background_color = {204, 204, 204, 255}, // p5.js default grey
    .has_fill = true,
    .has_stroke = true,
    .stroke_weight = 1.0f,
    .color_mode = PXL_COLOR_MODE_RGB,

    .text_size = 12.0f,
    .text_align = PXL_ALIGN_LEFT,

    .transform_stack_top = -1,
    .current_transform = {1, 0, 0, 1, 0, 0}, // identity

    .mouseX = 0,
    .mouseY = 0,
    .mouse_pressed = false,
    .key_code = 0,
    .key_pressed = false,
};

// ============================================================
//  Internal helpers
// ============================================================

// Convert HSB [0..255] to RGB SDL_Color
static SDL_Color hsb_to_rgb(float h, float s, float b, float a) {
    // h in [0,360), s,b in [0,255]
    float H = h;
    float S = s / 255.0f;
    float V = b / 255.0f;

    float r, g, bl;
    if (S == 0.0f) {
        r = g = bl = V;
    } else {
        H = fmodf(H, 360.0f) / 60.0f;
        int i = (int) H;
        float f = H - (float) i;
        float p = V * (1.0f - S);
        float q = V * (1.0f - S * f);
        float t = V * (1.0f - S * (1.0f - f));
        switch (i) {
            case 0: r = V;
                g = t;
                bl = p;
                break;
            case 1: r = q;
                g = V;
                bl = p;
                break;
            case 2: r = p;
                g = V;
                bl = t;
                break;
            case 3: r = p;
                g = q;
                bl = V;
                break;
            case 4: r = t;
                g = p;
                bl = V;
                break;
            default: r = V;
                g = p;
                bl = q;
                break;
        }
    }
    return (SDL_Color){
        (Uint8) (r * 255.0f),
        (Uint8) (g * 255.0f),
        (Uint8) (bl * 255.0f),
        (Uint8) (a)
    };
}

// Resolve a color through the current colorMode
static SDL_Color resolve_color(float c1, float c2, float c3, float a) {
    if (pxl_context.color_mode == PXL_COLOR_MODE_HSB) {
        return hsb_to_rgb(c1, c2, c3, (Uint8) a);
    }
    return (SDL_Color){(Uint8) c1, (Uint8) c2, (Uint8) c3, (Uint8) a};
}

// Apply the current transform to a point
static void transform_point(float x, float y, float* out_x, float* out_y) {
    PxlMatrix* m = &pxl_context.current_transform;
    *out_x = m->a * x + m->c * y + m->tx;
    *out_y = m->b * x + m->d * y + m->ty;
}

// Multiply two matrices: result = a * b
static PxlMatrix mat_mul(PxlMatrix a, PxlMatrix b) {
    return (PxlMatrix){
        .a = a.a * b.a + a.c * b.b,
        .b = a.b * b.a + a.d * b.b,
        .c = a.a * b.c + a.c * b.d,
        .d = a.b * b.c + a.d * b.d,
        .tx = a.a * b.tx + a.c * b.ty + a.tx,
        .ty = a.b * b.tx + a.d * b.ty + a.ty,
    };
}

// Set the SDL draw color to the stroke color
static void apply_stroke_color(void) {
    SDL_SetRenderDrawColor(pxl_context.renderer,
                           pxl_context.stroke_color.r, pxl_context.stroke_color.g,
                           pxl_context.stroke_color.b, pxl_context.stroke_color.a);
}

// Set the SDL draw color to the fill color
static void apply_fill_color(void) {
    SDL_SetRenderDrawColor(pxl_context.renderer,
                           pxl_context.fill_color.r, pxl_context.fill_color.g,
                           pxl_context.fill_color.b, pxl_context.fill_color.a);
}

// Draw a thick stroke line by rendering a filled rectangle along the line
static void draw_thick_line(float x1, float y1, float x2, float y2, float weight) {
    if (weight <= 1.0f) {
        SDL_RenderLine(pxl_context.renderer, x1, y1, x2, y2);
        return;
    }
    // Build a rectangle perpendicular to the line direction
    float dx = x2 - x1, dy = y2 - y1;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 0.001f) return;
    float nx = -dy / len * (weight * 0.5f);
    float ny = dx / len * (weight * 0.5f);

    SDL_Vertex verts[4] = {
        {
            {x1 + nx, y1 + ny},
            {
                pxl_context.stroke_color.r / 255.f, pxl_context.stroke_color.g / 255.f,
                pxl_context.stroke_color.b / 255.f, pxl_context.stroke_color.a / 255.f
            },
            {0, 0}
        },
        {
            {x1 - nx, y1 - ny},
            {
                pxl_context.stroke_color.r / 255.f, pxl_context.stroke_color.g / 255.f,
                pxl_context.stroke_color.b / 255.f, pxl_context.stroke_color.a / 255.f
            },
            {0, 0}
        },
        {
            {x2 + nx, y2 + ny},
            {
                pxl_context.stroke_color.r / 255.f, pxl_context.stroke_color.g / 255.f,
                pxl_context.stroke_color.b / 255.f, pxl_context.stroke_color.a / 255.f
            },
            {0, 0}
        },
        {
            {x2 - nx, y2 - ny},
            {
                pxl_context.stroke_color.r / 255.f, pxl_context.stroke_color.g / 255.f,
                pxl_context.stroke_color.b / 255.f, pxl_context.stroke_color.a / 255.f
            },
            {0, 0}
        },
    };
    int indices[6] = {0, 1, 2, 1, 2, 3};
    SDL_RenderGeometry(pxl_context.renderer, NULL, verts, 4, indices, 6);
}

// ============================================================
//  Ellipse engine  (used by both ellipse() and circle())
//  Draws a filled and/or stroked ellipse using triangle fan + outline
// ============================================================
#define PXL_ELLIPSE_SEGMENTS 64

static void ellipse_engine(float cx, float cy, float rx, float ry) {
    // Transform center
    float tcx, tcy;
    transform_point(cx, cy, &tcx, &tcy);

    // Build screen-space points — scale radii by the transform's scale component
    // For a uniform scale this is fine; for skew, approximate with average
    PxlMatrix* m = &pxl_context.current_transform;
    float scale_x = sqrtf(m->a * m->a + m->b * m->b);
    float scale_y = sqrtf(m->c * m->c + m->d * m->d);
    float trx = rx * scale_x;
    float try_ = ry * scale_y;

    const int N = PXL_ELLIPSE_SEGMENTS;

    if (pxl_context.has_fill) {
        apply_fill_color();
        SDL_Vertex verts[N + 1];
        int indices[N * 3];

        SDL_FColor fc = {
            pxl_context.fill_color.r / 255.f,
            pxl_context.fill_color.g / 255.f,
            pxl_context.fill_color.b / 255.f,
            pxl_context.fill_color.a / 255.f
        };

        // Center vertex
        verts[0].position.x = tcx;
        verts[0].position.y = tcy;
        verts[0].color = fc;
        verts[0].tex_coord = (SDL_FPoint){0, 0};

        for (int i = 0; i < N; i++) {
            float angle = (float) i / N * 2.0f * (float) M_PI;
            verts[i + 1].position.x = tcx + cosf(angle) * trx;
            verts[i + 1].position.y = tcy + sinf(angle) * try_;
            verts[i + 1].color = fc;
            verts[i + 1].tex_coord = (SDL_FPoint){0, 0};
            indices[i * 3 + 0] = 0;
            indices[i * 3 + 1] = i + 1;
            indices[i * 3 + 2] = (i + 1) % N + 1;
        }
        SDL_RenderGeometry(pxl_context.renderer, NULL, verts, N + 1, indices, N * 3);
    }

    if (pxl_context.has_stroke) {
        for (int i = 0; i < N; i++) {
            float a1 = (float) i / N * 2.0f * (float) M_PI;
            float a2 = (float) (i + 1) / N * 2.0f * (float) M_PI;
            float x1 = tcx + cosf(a1) * trx, y1 = tcy + sinf(a1) * try_;
            float x2 = tcx + cosf(a2) * trx, y2 = tcy + sinf(a2) * try_;
            apply_stroke_color();
            draw_thick_line(x1, y1, x2, y2, pxl_context.stroke_weight);
        }
    }
}

// ============================================================
//  Lifecycle
// ============================================================

void pxl_init(void) {
    if (pxl_context.width == 0 || pxl_context.height == 0)
        pxl_runtime_error("Canvas size must be set before pxl_init");

    if (!SDL_Init(SDL_INIT_VIDEO))
        pxl_runtime_error(SDL_GetError());

    if (!SDL_CreateWindowAndRenderer(pxl_context.window_title,
                                     pxl_context.width, pxl_context.height, 0,
                                     &pxl_context.window, &pxl_context.renderer))
        pxl_runtime_error(SDL_GetError());

    SDL_SetRenderDrawBlendMode(pxl_context.renderer, SDL_BLENDMODE_BLEND);
}

void pxl_run(void (*setup_fn)(void), void (*draw_fn)(void)) {
    if (pxl_context.fps == 0)
        pxl_runtime_error("pxl_run: FPS must be greater than 0");

    pxl_init();
    if (setup_fn) setup_fn();

    if (!draw_fn) {
        pxl_quit();
        return;
    }

    const Uint64 target_ms = (Uint64) (1000.0 / pxl_context.fps + 0.5);
    SDL_Event event;
    bool running = true;

    while (running) {
        const Uint64 t0 = SDL_GetTicks();

        // Reset per-frame input flags
        pxl_context.key_pressed = false;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    pxl_context.mouseX = event.motion.x;
                    pxl_context.mouseY = event.motion.y;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    pxl_context.mouse_pressed = true;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    pxl_context.mouse_pressed = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    pxl_context.key_pressed = true;
                    pxl_context.key_code = event.key.key;
                    break;
                default:
                    break;
            }
        }

        if (pxl_context.looping) {
            draw_fn();
            if (!SDL_RenderPresent(pxl_context.renderer))
                pxl_runtime_error(SDL_GetError());
        } else {
            SDL_Delay(16); // idle when not looping
        }

        const Uint64 elapsed = SDL_GetTicks() - t0;
        if (elapsed < target_ms)
            SDL_Delay((Uint32) (target_ms - elapsed));
    }

    pxl_quit();
}

void pxl_quit(void) {
    if (pxl_context.renderer) SDL_DestroyRenderer(pxl_context.renderer);
    if (pxl_context.window) SDL_DestroyWindow(pxl_context.window);
    SDL_Quit();
}

// ============================================================
//  Pre-setup configuration
// ============================================================

void pxl_set_canvas_size(int width, int height) {
    pxl_context.width = width;
    pxl_context.height = height;
    if (pxl_context.window)
        SDL_SetWindowSize(pxl_context.window, width, height);
}

void pxl_set_frames_per_second(Uint32 fps) {
    pxl_context.fps = fps;
}

void pxl_set_window_title(const char* title) {
    pxl_context.window_title = title;
    if (pxl_context.window)
        SDL_SetWindowTitle(pxl_context.window, title);
}

// ============================================================
//  Structure
// ============================================================

void loop(void) { pxl_context.looping = true; }
void noLoop(void) { pxl_context.looping = false; }

// ============================================================
//  Color / settings
// ============================================================

void background(float r, float g, float b) {
    SDL_Color c = resolve_color(r, g, b, 255);
    SDL_SetRenderDrawColor(pxl_context.renderer, c.r, c.g, c.b, 255);
    SDL_RenderClear(pxl_context.renderer);
    // Restore blend mode after clear
    SDL_SetRenderDrawBlendMode(pxl_context.renderer, SDL_BLENDMODE_BLEND);
}

void fill(float r, float g, float b) {
    pxl_context.fill_color = resolve_color(r, g, b, 255);
    pxl_context.has_fill = true;
}

void fill_a(float r, float g, float b, float a) {
    pxl_context.fill_color = resolve_color(r, g, b, a);
    pxl_context.has_fill = true;
}

void noFill(void) {
    pxl_context.has_fill = false;
}

void stroke(float r, float g, float b) {
    pxl_context.stroke_color = resolve_color(r, g, b, 255);
    pxl_context.has_stroke = true;
}

void stroke_a(float r, float g, float b, float a) {
    pxl_context.stroke_color = resolve_color(r, g, b, a);
    pxl_context.has_stroke = true;
}

void noStroke(void) {
    pxl_context.has_stroke = false;
}

void strokeWeight(float weight) {
    pxl_context.stroke_weight = weight > 0 ? weight : 0;
}

void colorMode(PxlColorMode mode) {
    pxl_context.color_mode = mode;
}

// ============================================================
//  2D Primitives
// ============================================================

void rect(float x, float y, float w, float h) {
    // Transform all 4 corners
    float tx0, ty0, tx1, ty1, tx2, ty2, tx3, ty3;
    transform_point(x, y, &tx0, &ty0);
    transform_point(x + w, y, &tx1, &ty1);
    transform_point(x + w, y + h, &tx2, &ty2);
    transform_point(x, y + h, &tx3, &ty3);

    if (pxl_context.has_fill) {
        SDL_FColor fc = {
            pxl_context.fill_color.r / 255.f,
            pxl_context.fill_color.g / 255.f,
            pxl_context.fill_color.b / 255.f,
            pxl_context.fill_color.a / 255.f
        };
        SDL_Vertex verts[4] = {
            {{tx0, ty0}, fc, {0, 0}},
            {{tx1, ty1}, fc, {0, 0}},
            {{tx2, ty2}, fc, {0, 0}},
            {{tx3, ty3}, fc, {0, 0}},
        };
        int indices[6] = {0, 1, 2, 0, 2, 3};
        SDL_RenderGeometry(pxl_context.renderer, NULL, verts, 4, indices, 6);
    }

    if (pxl_context.has_stroke) {
        apply_stroke_color();
        draw_thick_line(tx0, ty0, tx1, ty1, pxl_context.stroke_weight);
        draw_thick_line(tx1, ty1, tx2, ty2, pxl_context.stroke_weight);
        draw_thick_line(tx2, ty2, tx3, ty3, pxl_context.stroke_weight);
        draw_thick_line(tx3, ty3, tx0, ty0, pxl_context.stroke_weight);
    }
}

void circle(float x, float y, float d) {
    ellipse(x, y, d, d);
}

void ellipse(float cx, float cy, float w, float h) {
    ellipse_engine(cx, cy, w * 0.5f, h * 0.5f);
}

void line(float x1, float y1, float x2, float y2) {
    if (!pxl_context.has_stroke) return;
    float tx1, ty1, tx2, ty2;
    transform_point(x1, y1, &tx1, &ty1);
    transform_point(x2, y2, &tx2, &ty2);
    apply_stroke_color();
    draw_thick_line(tx1, ty1, tx2, ty2, pxl_context.stroke_weight);
}

void triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    float tx1, ty1, tx2, ty2, tx3, ty3;
    transform_point(x1, y1, &tx1, &ty1);
    transform_point(x2, y2, &tx2, &ty2);
    transform_point(x3, y3, &tx3, &ty3);

    if (pxl_context.has_fill) {
        SDL_FColor fc = {
            pxl_context.fill_color.r / 255.f,
            pxl_context.fill_color.g / 255.f,
            pxl_context.fill_color.b / 255.f,
            pxl_context.fill_color.a / 255.f
        };
        SDL_Vertex verts[3] = {
            {{tx1, ty1}, fc, {0, 0}},
            {{tx2, ty2}, fc, {0, 0}},
            {{tx3, ty3}, fc, {0, 0}},
        };
        int indices[3] = {0, 1, 2};
        SDL_RenderGeometry(pxl_context.renderer, NULL, verts, 3, indices, 3);
    }

    if (pxl_context.has_stroke) {
        apply_stroke_color();
        draw_thick_line(tx1, ty1, tx2, ty2, pxl_context.stroke_weight);
        draw_thick_line(tx2, ty2, tx3, ty3, pxl_context.stroke_weight);
        draw_thick_line(tx3, ty3, tx1, ty1, pxl_context.stroke_weight);
    }
}

void point(float x, float y) {
    if (!pxl_context.has_stroke) return;
    float tx, ty;
    transform_point(x, y, &tx, &ty);
    apply_stroke_color();
    float r = pxl_context.stroke_weight * 0.5f;
    if (r <= 0.5f) {
        SDL_RenderPoint(pxl_context.renderer, tx, ty);
    } else {
        // Draw as a small filled circle
        ellipse_engine(tx, ty, r, r);
    }
}

// ============================================================
//  Transforms
// ============================================================

void translate(float x, float y) {
    PxlMatrix t = {1, 0, 0, 1, x, y};
    pxl_context.current_transform = mat_mul(pxl_context.current_transform, t);
}

void rotate(float angle) {
    float c = cosf(angle), s = sinf(angle);
    PxlMatrix r = {c, s, -s, c, 0, 0};
    pxl_context.current_transform = mat_mul(pxl_context.current_transform, r);
}

void scale_xy(float sx, float sy) {
    PxlMatrix s = {sx, 0, 0, sy, 0, 0};
    pxl_context.current_transform = mat_mul(pxl_context.current_transform, s);
}

void push(void) {
    int top = pxl_context.transform_stack_top;
    if (top + 1 >= PXL_MAX_TRANSFORM_STACK)
        pxl_runtime_error("push(): transform stack overflow");
    pxl_context.transform_stack[top + 1] = pxl_context.current_transform;
    pxl_context.transform_stack_top = top + 1;
}

void pop(void) {
    int top = pxl_context.transform_stack_top;
    if (top < 0)
        pxl_runtime_error("pop(): transform stack underflow");
    pxl_context.current_transform = pxl_context.transform_stack[top];
    pxl_context.transform_stack_top = top - 1;
}

// ============================================================
//  Math
// ============================================================

float map_val(float value, float start1, float stop1, float start2, float stop2) {
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

float lerp_val(float start, float stop, float amt) {
    return start + (stop - start) * amt;
}

float constrain_val(float n, float low, float high) {
    if (n < low) return low;
    if (n > high) return high;
    return n;
}

float dist_val(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1, dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

float random_val(float low, float high) {
    return low + ((float) rand() / (float) RAND_MAX) * (high - low);
}

// ---- Value noise (smooth, [0,1]) ----
// Based on Ken Perlin's smooth interpolation + random gradient table

#define PXL_NOISE_TABLE_SIZE 256
static float noise_table[PXL_NOISE_TABLE_SIZE];
static int noise_perm[PXL_NOISE_TABLE_SIZE * 2];
static bool noise_inited = false;

static void noise_init(void) {
    if (noise_inited) return;
    for (int i = 0; i < PXL_NOISE_TABLE_SIZE; i++) {
        noise_table[i] = (float) rand() / (float) RAND_MAX;
        noise_perm[i] = i;
    }
    // Shuffle
    for (int i = PXL_NOISE_TABLE_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = noise_perm[i];
        noise_perm[i] = noise_perm[j];
        noise_perm[j] = tmp;
    }
    for (int i = 0; i < PXL_NOISE_TABLE_SIZE; i++)
        noise_perm[i + PXL_NOISE_TABLE_SIZE] = noise_perm[i];
    noise_inited = true;
}

static float smooth(float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

float noise_val(float x) {
    noise_init();
    int xi = (int) floorf(x) & (PXL_NOISE_TABLE_SIZE - 1);
    float xf = x - floorf(x);
    float u = smooth(xf);
    float a = noise_table[noise_perm[xi]];
    float b = noise_table[noise_perm[xi + 1]];
    return a + u * (b - a);
}

float noise_val2(float x, float y) {
    noise_init();
    int xi = (int) floorf(x) & (PXL_NOISE_TABLE_SIZE - 1);
    int yi = (int) floorf(y) & (PXL_NOISE_TABLE_SIZE - 1);
    float xf = x - floorf(x), yf = y - floorf(y);
    float u = smooth(xf), v = smooth(yf);

    float aa = noise_table[noise_perm[noise_perm[xi] + yi]];
    float ab = noise_table[noise_perm[noise_perm[xi] + yi + 1]];
    float ba = noise_table[noise_perm[noise_perm[xi + 1] + yi]];
    float bb = noise_table[noise_perm[noise_perm[xi + 1] + yi + 1]];

    float x1 = aa + u * (ba - aa);
    float x2 = ab + u * (bb - ab);
    return x1 + v * (x2 - x1);
}
