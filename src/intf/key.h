#pragma once

#include <stdbool.h>

typedef struct
{
    unsigned char ascii;
    unsigned char scancode;
    bool shift;
    bool alt_left;
    bool alt_right;
    bool ctrl_left;
    bool ctrl_right;
    bool caps_lock;
    bool num_lock;
    bool scroll_lock;
} Key;

bool key_valid(Key key);
void invalidate_key(Key key);

bool key_alt(Key key);
bool key_ctrl(Key key);

// scan codes of special keys
typedef enum
{
    key_f1 = 0x3b,
    key_f2 = 0x3c,
    key_f3 = 0x3d,
    key_f4 = 0x3e,
    key_f5 = 0x3f,
    key_f6 = 0x40,
    key_f7 = 0x41,
    key_f8 = 0x42,
    key_f9 = 0x43,
    key_f10 = 0x44,
    key_f11 = 0x57,
    key_f12 = 0x58,
    key_nul = 0x00,
    key_esc = 0x01,
    key_tab = 0x0F,
    key_bsp = 0x0E,
    key_ins = 0x52,
    key_del = 0x53,
    key_home = 0x47,
    key_end = 0x4F,
    key_pgup = 0x49,
    key_pgdwn = 0x51,
    // prtscr has same key.scancode as numpad-* (Key key, check for key.ascii)
    key_prtscr = 0x37,
    // pause has same key.scancode as numlock (Key key, pause has lctrl set)
    key_pause = 0x45,
    key_lgui = 0x5B,
    key_rgui = 0x5C,
    key_apps = 0x5D,
    key_acpi_power = 0x5E,
    key_acpi_sleep = 0x5F,
    key_acpi_wake = 0x63,
    key_up = 72,
    key_down = 80,
    key_left = 75,
    key_right = 77,
    key_div = 8
} key_scancode;
