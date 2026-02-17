#ifndef SYSCALL_H
#define SYSCALL_H

#include "stdint.h"

#define SYS_EXIT       0x6001
#define SYS_WRITE      0x6002
#define SYS_READ       0x6003
#define SYS_OPEN       0x6004
#define SYS_CLOSE      0x6005
#define SYS_CREATE_WIN 0x6010
#define SYS_DRAW_RECT  0x6011
#define SYS_GET_EVENT  0x6012
#define SYS_BEEP       0x6013
#define SYS_GET_TICKS  0x6014
#define SYS_SET_WIN_TITLE 0x6015

#ifndef __ASSEMBLER__

int sys_exit(int status);
int sys_write(int fd, const void *buf, int count);
int sys_open(const char *path, int flags);
int sys_create_window(const char *title, int x, int y, int w, int h);
int sys_draw_rect(int win_id, int x, int y, int w, int h, uint32_t color);
int sys_get_event(int win_id, void *event);
int sys_beep(uint32_t freq, uint32_t duration);
uint32_t sys_get_ticks();
int sys_set_window_title(int win_id, const char *title);

#endif
#endif