#ifndef WINDOW_H
#define WINDOW_H

#include "stdint.h"

#define MAX_WINDOWS      32
#define MAX_TITLE_LEN    64
#define WINDOW_EVENT_KEY 1
#define WINDOW_EVENT_MOUSE 2
#define WINDOW_EVENT_CLOSE 3

typedef struct {
    uint32_t id;
    char title[MAX_TITLE_LEN];
    int32_t x, y;
    uint32_t width, height;
    uint8_t *framebuffer;
    uint8_t visible;
    uint32_t z_order;
    uint32_t event_queue; // stub
} window_t;

typedef struct {
    uint32_t type;
    uint32_t window_id;
    union {
        struct { uint32_t keycode; } key;
        struct { int32_t x, y; uint32_t buttons; } mouse;
    };
} window_event_t;

extern window_t windows[MAX_WINDOWS];

void wm_init(uint32_t screen_width, uint32_t screen_height, uint32_t *framebuffer);
uint32_t wm_create_window(const char *title, int32_t x, int32_t y, uint32_t w, uint32_t h);
void wm_destroy_window(uint32_t win_id);
void wm_composite();
void wm_send_event(uint32_t win_id, window_event_t *evt);
int wm_get_event(uint32_t win_id, window_event_t *evt);
void wm_handle_mouse(int32_t dx, int32_t dy, uint8_t buttons);
void wm_handle_key(uint32_t keycode, uint8_t pressed);

#endif