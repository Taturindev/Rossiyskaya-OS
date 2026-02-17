#include "window.h"
#include "graphics.h"
#include "kheap.h"
#include "string.h"

window_t windows[MAX_WINDOWS];
static uint32_t next_window_id = 1;
static uint32_t screen_w, screen_h;
static uint32_t *screen_fb;
static int32_t mouse_x = 400, mouse_y = 300;
static uint8_t mouse_buttons = 0;

int dragging_win = 0;
int drag_off_x = 0, drag_off_y = 0;

void wm_init(uint32_t screen_width, uint32_t screen_height, uint32_t *framebuffer) {
    screen_w = screen_width;
    screen_h = screen_height;
    screen_fb = framebuffer;
    for (int i = 0; i < MAX_WINDOWS; i++) {
        windows[i].id = 0;
    }
    windows[0].id = 0;
    strcpy(windows[0].title, "Desktop");
    windows[0].x = 0;
    windows[0].y = 0;
    windows[0].width = screen_w;
    windows[0].height = screen_h - 40;
    windows[0].framebuffer = (uint8_t*)kmalloc(screen_w * screen_h * 4);
    windows[0].visible = 1;
    windows[0].z_order = 0;
    fill_rect_fb((uint32_t*)windows[0].framebuffer, screen_w, screen_h, 0, 0, screen_w, screen_h - 40, 0x008080);
}

uint32_t wm_create_window(const char *title, int32_t x, int32_t y, uint32_t w, uint32_t h) {
    int slot = -1;
    for (int i = 1; i < MAX_WINDOWS; i++) {
        if (windows[i].id == 0) { slot = i; break; }
    }
    if (slot == -1) return 0;
    windows[slot].id = next_window_id++;
    strcpy(windows[slot].title, title);
    windows[slot].x = x;
    windows[slot].y = y;
    windows[slot].width = w;
    windows[slot].height = h;
    windows[slot].framebuffer = (uint8_t*)kmalloc(w * h * 4);
    windows[slot].visible = 1;
    windows[slot].z_order = slot;
    fill_rect_fb((uint32_t*)windows[slot].framebuffer, w, h, 0, 0, w, h, 0xFFFFFF);
    return windows[slot].id;
}

void wm_destroy_window(uint32_t win_id) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].id == win_id) {
            kfree(windows[i].framebuffer);
            windows[i].id = 0;
            return;
        }
    }
}

void wm_composite() {
    memcpy(screen_fb, windows[0].framebuffer, screen_w * (screen_h - 40) * 4);
    for (int z = 1; z < MAX_WINDOWS; z++) {
        for (int i = 1; i < MAX_WINDOWS; i++) {
            if (windows[i].id && windows[i].visible && windows[i].z_order == z) {
                for (uint32_t wy = 0; wy < windows[i].height; wy++) {
                    uint32_t screen_y = windows[i].y + wy;
                    if (screen_y >= screen_h - 40) continue;
                    uint32_t *src = (uint32_t*)(windows[i].framebuffer + wy * windows[i].width * 4);
                    uint32_t *dst = screen_fb + screen_y * screen_w + windows[i].x;
                    for (uint32_t wx = 0; wx < windows[i].width; wx++) {
                        uint32_t screen_x = windows[i].x + wx;
                        if (screen_x < screen_w) {
                            dst[wx] = src[wx];
                        }
                    }
                }
            }
        }
    }
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            if (i*i + j*j <= 4) {
                uint32_t mx = mouse_x + i;
                uint32_t my = mouse_y + j;
                if (mx < screen_w && my < screen_h - 40) {
                    screen_fb[my * screen_w + mx] = 0xFFFFFF;
                }
            }
        }
    }
}

void wm_handle_mouse(int32_t dx, int32_t dy, uint8_t buttons) {
    mouse_x += dx;
    mouse_y += dy;
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_x >= screen_w) mouse_x = screen_w - 1;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_y >= screen_h - 40) mouse_y = screen_h - 41;
    mouse_buttons = buttons;

    if (dragging_win) {
        if (buttons & 1) {
            for (int i = 0; i < MAX_WINDOWS; i++) {
                if (windows[i].id == dragging_win) {
                    windows[i].x = mouse_x - drag_off_x;
                    windows[i].y = mouse_y - drag_off_y;
                    if (windows[i].x < 0) windows[i].x = 0;
                    if (windows[i].x + windows[i].width > screen_w)
                        windows[i].x = screen_w - windows[i].width;
                    if (windows[i].y < 0) windows[i].y = 0;
                    if (windows[i].y + windows[i].height > screen_h - 40)
                        windows[i].y = screen_h - 40 - windows[i].height;
                    break;
                }
            }
            return;
        } else {
            dragging_win = 0;
        }
    }

    for (int z = MAX_WINDOWS - 1; z >= 0; z--) {
        for (int i = 0; i < MAX_WINDOWS; i++) {
            if (windows[i].id && windows[i].visible && windows[i].z_order == z) {
                if (mouse_x >= windows[i].x && mouse_x < windows[i].x + windows[i].width &&
                    mouse_y >= windows[i].y && mouse_y < windows[i].y + windows[i].height) {
                    if ((buttons & 1) && (mouse_y - windows[i].y < 20)) {
                        dragging_win = windows[i].id;
                        drag_off_x = mouse_x - windows[i].x;
                        drag_off_y = mouse_y - windows[i].y;
                    }
                    window_event_t evt;
                    evt.type = WINDOW_EVENT_MOUSE;
                    evt.window_id = windows[i].id;
                    evt.mouse.x = mouse_x - windows[i].x;
                    evt.mouse.y = mouse_y - windows[i].y;
                    evt.mouse.buttons = buttons;
                    wm_send_event(windows[i].id, &evt);
                    return;
                }
            }
        }
    }
}

void wm_handle_key(uint32_t keycode, uint8_t pressed) {
    for (int z = MAX_WINDOWS - 1; z >= 0; z--) {
        for (int i = 0; i < MAX_WINDOWS; i++) {
            if (windows[i].id && windows[i].visible && windows[i].z_order == z) {
                window_event_t evt;
                evt.type = WINDOW_EVENT_KEY;
                evt.window_id = windows[i].id;
                evt.key.keycode = keycode;
                wm_send_event(windows[i].id, &evt);
                return;
            }
        }
    }
}

void wm_send_event(uint32_t win_id, window_event_t *evt) {
    (void)win_id; (void)evt;
}

int wm_get_event(uint32_t win_id, window_event_t *evt) {
    (void)win_id; (void)evt;
    return 0;
}