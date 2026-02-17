#include "mouse.h"
#include "io.h"
#include "window.h"

static uint8_t mouse_cycle = 0;
static uint8_t mouse_byte[3];

static void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if ((inb(0x64) & 2) == 0) return;
        }
    } else {
        while (timeout--) {
            if (inb(0x64) & 1) return;
        }
    }
}

static void mouse_write(uint8_t data) {
    mouse_wait(0);
    outb(0x64, 0xD4);
    mouse_wait(0);
    outb(0x60, data);
}

static uint8_t mouse_read() {
    mouse_wait(1);
    return inb(0x60);
}

void mouse_init() {
    mouse_wait(0);
    outb(0x64, 0xA8);
    mouse_wait(0);
    outb(0x64, 0x20);
    uint8_t status = mouse_read();
    status |= 2;
    mouse_wait(0);
    outb(0x64, 0x60);
    mouse_wait(0);
    outb(0x60, status);
    mouse_write(0xF4);
    mouse_read();
}

void mouse_handler() {
    uint8_t data = inb(0x60);
    switch(mouse_cycle) {
        case 0:
            if (data & 8) {
                mouse_byte[0] = data;
                mouse_cycle = 1;
            }
            break;
        case 1:
            mouse_byte[1] = data;
            mouse_cycle = 2;
            break;
        case 2:
            mouse_byte[2] = data;
            uint8_t buttons = mouse_byte[0] & 7;
            int32_t dx = (int32_t)(mouse_byte[1]) - ((mouse_byte[0] << 4) & 0x100);
            int32_t dy = (int32_t)(mouse_byte[2]) - ((mouse_byte[0] << 3) & 0x100);
            dy = -dy;
            wm_handle_mouse(dx, dy, buttons);
            mouse_cycle = 0;
            break;
    }
}