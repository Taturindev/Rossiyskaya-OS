#include "kprintf.h"
#include "graphics.h"
#include "io.h"

static void kputc(char c) {
    if (framebuffer) {
        static uint32_t x = 0, y = 0;
        if (c == '\n') { x = 0; y += 10; }
        else {
            draw_char(c, x, y, COLOR_WHITE, COLOR_BLACK);
            x += 9;
            if (x >= fb_width - 9) { x = 0; y += 10; }
        }
        if (y >= fb_height - 40) y = 0;
    } else {
        char *video = (char*)0xB8000;
        static int pos = 0;
        if (c == '\n') pos += 80 - (pos % 80);
        else {
            video[pos*2] = c;
            video[pos*2+1] = 0x07;
            pos++;
        }
        if (pos >= 80*25) pos = 0;
    }
}

void kprintf(const char *format, ...) {
    (void)format;
    for (const char *p = format; *p; p++) kputc(*p);
}