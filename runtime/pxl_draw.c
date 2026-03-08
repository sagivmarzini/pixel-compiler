#include "pxl_draw.h"

#include <SDL3/SDL_log.h>

#include "pxl_graphics.h"

void pxl_draw_rect(int x, int y, int width, int height) {
    SDL_FRect rect = (SDL_FRect){x, y, width, height};
    if (!SDL_RenderFillRect(pxl_context.renderer, &rect)) {
        SDL_Log("Failed to draw a rect at (%d, %d): %s\n", x, y, SDL_GetError());
        pxl_quit();
    }
}

void pxl_draw_empty_rect(int x, int y, int width, int height) {
    SDL_FRect rect = (SDL_FRect){x, y, width, height};
    if (!SDL_RenderRect(pxl_context.renderer, &rect)) {
        SDL_Log("Failed to draw an empty rect at (%d, %d): %s\n", x, y, SDL_GetError());
        pxl_quit();
    }
}

// This midpoint algorithm is very slow. Best to use a polygon approach by drawing a polygon with ~30 segments.
static void pxl_circle_engine(int center_x, int center_y, int radius, bool filled) {
    int offset_x = radius;
    int offset_y = 0;
    int decision = 1 - offset_x;

    while (offset_x >= offset_y) {
        int result = 0;

        if (filled) {
            // Draw 4 horizontal lines to fill the circle
            result |= SDL_RenderLine(pxl_context.renderer, center_x - offset_x, center_y + offset_y,
                                     center_x + offset_x, center_y + offset_y);
            result |= SDL_RenderLine(pxl_context.renderer, center_x - offset_x, center_y - offset_y,
                                     center_x + offset_x, center_y - offset_y);
            result |= SDL_RenderLine(pxl_context.renderer, center_x - offset_y, center_y + offset_x,
                                     center_x + offset_y, center_y + offset_x);
            result |= SDL_RenderLine(pxl_context.renderer, center_x - offset_y, center_y - offset_x,
                                     center_x + offset_y, center_y - offset_x);
        } else {
            // Draw the 8 points of the circumference
            result |= SDL_RenderPoint(pxl_context.renderer, center_x + offset_x, center_y + offset_y);
            result |= SDL_RenderPoint(pxl_context.renderer, center_x - offset_x, center_y + offset_y);
            result |= SDL_RenderPoint(pxl_context.renderer, center_x + offset_x, center_y - offset_y);
            result |= SDL_RenderPoint(pxl_context.renderer, center_x - offset_x, center_y - offset_y);
            result |= SDL_RenderPoint(pxl_context.renderer, center_x + offset_y, center_y + offset_x);
            result |= SDL_RenderPoint(pxl_context.renderer, center_x - offset_y, center_y + offset_x);
            result |= SDL_RenderPoint(pxl_context.renderer, center_x + offset_y, center_y - offset_x);
            result |= SDL_RenderPoint(pxl_context.renderer, center_x - offset_y, center_y - offset_x);
        }

        if (result < 0) {
            SDL_Log("Circle drawing failed: %s", SDL_GetError());
            pxl_quit();
        }

        offset_y++;
        if (decision < 0) {
            decision += 2 * offset_y + 1;
        } else {
            offset_x--;
            decision += 2 * (offset_y - offset_x) + 1;
        }
    }
}

void pxl_draw_circle_filled(int x, int y, int r) {
    pxl_circle_engine(x, y, r, true);
}

void pxl_draw_circle_empty(int x, int y, int r) {
    pxl_circle_engine(x, y, r, false);
}

void pxl_draw_line(int x1, int y1, int x2, int y2) {
    if (!SDL_RenderLine(pxl_context.renderer, x1, y1, x2, y2)) {
        SDL_Log("Failed to draw a line: %s", SDL_GetError());
        pxl_quit();
    }
}

void pxl_draw_point(int x, int y) {
    if (!SDL_RenderPoint(pxl_context.renderer, x, y)) {
        SDL_Log("Failed to draw a point at (%d, %d): %s", x, y, SDL_GetError());
        pxl_quit();
    }
}
