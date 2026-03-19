#pragma once
#include <SDL3/SDL_render.h>

void pxl_draw_rect(int x, int y, int width, int height);

void pxl_draw_empty_rect(int x, int y, int width, int height);

void pxl_draw_circle_filled(int x, int y, int radius);

void pxl_draw_circle_empty(int x, int y, int radius);

void pxl_draw_line(int x1, int y1, int x2, int y2);

void pxl_draw_point(int x, int y);

SDL_Texture* pxl_load_image(const char* img_path); // call in setup()

void pxl_draw_image(SDL_Texture* texture, int x, int y); // call in draw()
