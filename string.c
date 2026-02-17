#include "string.h"

void *memcpy(void *dest, const void *src, uint32_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;
    for (uint32_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

void *memset(void *s, int c, uint32_t n) {
    uint8_t *p = s;
    for (uint32_t i = 0; i < n; i++) p[i] = (uint8_t)c;
    return s;
}

int strlen(const char *s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}