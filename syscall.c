#include "syscall.h"
#include "window.h"
#include "process.h"
#include "graphics.h"
#include "timer.h"

extern void beep(uint32_t frequency, uint32_t duration_ms);

uint32_t syscall_handler(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi) {
    switch(eax) {
        case SYS_EXIT:
            exit_process(ebx);
            return 0;
        case SYS_WRITE: {
            int fd = ebx;
            const char *buf = (const char*)ecx;
            int count = edx;
            if (fd == 1) {
                // вывод на экран (можно реализовать через kprintf)
                return count;
            }
            return -1;
        }
        case SYS_CREATE_WIN: {
            const char *title = (const char*)ebx;
            int x = ecx, y = edx, w = esi, h = edi;
            return wm_create_window(title, x, y, w, h);
        }
        case SYS_DRAW_RECT: {
            int win_id = ebx;
            int x = ecx, y = edx, w = esi, h = edi;
            uint32_t color = (uint32_t)esi >> 16; // hack
            for (int i = 0; i < MAX_WINDOWS; i++) {
                if (windows[i].id == win_id) {
                    fill_rect_fb((uint32_t*)windows[i].framebuffer,
                                 windows[i].width, windows[i].height,
                                 x, y, w, h, color);
                    return 0;
                }
            }
            return -1;
        }
        case SYS_BEEP:
            beep(ebx, ecx);
            return 0;
        case SYS_GET_TICKS:
            return get_ticks();
        case SYS_SET_WIN_TITLE: {
            int win_id = ebx;
            const char *title = (const char*)ecx;
            for (int i = 0; i < MAX_WINDOWS; i++) {
                if (windows[i].id == win_id) {
                    strcpy(windows[i].title, title);
                    return 0;
                }
            }
            return -1;
        }
        default:
            return -1;
    }
}