#pragma once

#include <stddef.h>

typedef enum CGA_Color
{
    CGA_F_BLACK = 0,
    CGA_F_BLUE = 1,
    CGA_F_GREEN = 2,
    CGA_F_CYAN = 3,
    CGA_F_RED = 4,
    CGA_F_MAGENTA = 5,
    CGA_F_BROWN = 6,
    CGA_F_LIGHT_GRAY = 7,
    CGA_F_DARK_GRAY = 8,
    CGA_F_LIGHT_BLUE = 9,
    CGA_F_LIGHT_GREEN = 10,
    CGA_F_LIGHT_CYAN = 11,
    CGA_F_LIGHT_RED = 12,
    CGA_F_LIGHT_MAGENTA = 13,
    CGA_F_YELLOW = 14,
    CGA_F_WHITE = 15,
    CGA_B_BLACK = 0,
    CGA_B_BLUE = 16,
    CGA_B_GREEN = 32,
    CGA_B_CYAN = 48,
    CGA_B_RED = 64,
    CGA_B_MAGENTA = 80,
    CGA_B_BROWN = 96,
    CGA_B_LIGHT_GRAY = 112,
    CGA_BLINK = 128,
    CGA_DEFAULT_COLOR = CGA_F_LIGHT_GRAY | CGA_B_BLACK,
} CGA_Color;

void CGA_clear();
void CGA_show(size_t x, size_t y, char c);
void CGA_setpos(size_t x, size_t y);
void CGA_getpos(size_t *x, size_t *y);
void CGA_scroll();
void CGA_putchar(char c);
void CGA_puts(const char *s);
void CGA_set_color(CGA_Color c);
