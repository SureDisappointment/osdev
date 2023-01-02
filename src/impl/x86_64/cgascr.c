#include "cgascr.h"
#include "stdlib/algorithm.h"
#include "stdlib/assert.h"
#include "panic.h"
#include <stdint.h>

static const size_t WIDTH = 80;
static const size_t HEIGHT = 25;

typedef struct
{
    char c;
    uint8_t color;
} glyph;
static const glyph clear_glyph = {0, CGA_DEFAULT_COLOR};

glyph *screen = (glyph*) 0xb8000;
size_t cursor_x = 0;
size_t cursor_y = 0;

CGA_Color color = CGA_DEFAULT_COLOR;

void show_glyph(size_t x, size_t y, glyph g)
{
    assert(y < HEIGHT && x < WIDTH, "CGA video memory indexed out of bounds");
    screen[y * WIDTH + x] = g;
}

void clear_row(size_t row)
{
    for(size_t col = 0; col < WIDTH; col++)
        show_glyph(col, row, clear_glyph);
}

void CGA_clear()
{
    for(size_t i = 0; i < HEIGHT; i++)
        clear_row(i);
}

void CGA_show(size_t x, size_t y, char c)
{
    glyph g = {c, color};
    show_glyph(x, y, g);
}

void CGA_setpos(size_t x, size_t y)
{
    assert(y < HEIGHT && x < WIDTH, "CGA screen indexed out of bounds");
    cursor_x = x;
    cursor_y = y;
}

void CGA_getpos(size_t *x, size_t *y)
{
    *x = cursor_x;
    *y = cursor_y;
}

void CGA_scroll()
{
    memcpy(screen, &screen[WIDTH], (HEIGHT - 1) * WIDTH);
    clear_row(HEIGHT - 1);
}

void handle_control_char(char c)
{
    switch(c)
    {
    case '\a': /* play a sound */ break;
    case '\b':
        // Don't do anything if cursor is in upper left corner
        if (cursor_x == 0 && cursor_y == 0)
            break;

        if (cursor_x == 0)
        {
            cursor_x = WIDTH - 1;
            if (cursor_y != 0)
                cursor_y--;
        }
        else
            cursor_x--;
        // Delete char
        show_glyph(cursor_x, cursor_y, clear_glyph);
        break;
    case '\t':
        CGA_putchar(' ');
        while (cursor_x % 4 != 0 && cursor_x < 80)
            CGA_putchar(' ');
        break;
    case '\n':
        cursor_x = 0;
    case '\v':
    case '\f':
        if(cursor_y < HEIGHT - 1)
        {
            cursor_y++;
            break;
        }
        CGA_scroll();
        break;
    case '\r': cursor_x = 0; break;
    default:
        // Char is not printable, ignore for now
        break;
    }
}

void CGA_putchar(char c)
{
    if(cursor_x == WIDTH)
        handle_control_char('\n');

    if(c < ' ')
    {
        handle_control_char(c);
        return;
    }

    CGA_show(cursor_x, cursor_y, c);
    cursor_x++;
}

void CGA_puts(const char *s)
{
    while(*s)
        CGA_putchar(*s++);
}

void CGA_set_color(CGA_Color c)
{
    color = c;
}
