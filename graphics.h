#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "stdint.h"
#include "multiboot.h"

extern uint32_t *framebuffer;
extern uint32_t fb_width, fb_height, fb_pitch, fb_bpp;

void init_graphics(multiboot_info_t *mb_info);
void put_pixel(uint32_t x, uint32_t y, uint32_t color);
void fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void fill_rect_fb(uint32_t *buf, uint32_t buf_w, uint32_t buf_h,
                  uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void draw_char(char c, uint32_t x, uint32_t y, uint32_t color, uint32_t bg_color);
void draw_string(const char *str, uint32_t x, uint32_t y, uint32_t color, uint32_t bg_color);

#define COLOR_BLACK   0x000000
#define COLOR_WHITE   0xFFFFFF
#define COLOR_RED     0xFF0000
#define COLOR_GREEN   0x00FF00
#define COLOR_BLUE    0x0000FF
#define COLOR_CYAN    0x00FFFF
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_YELLOW  0xFFFF00

#endif