#include "device/ps2_keyboard.h"
#include "plugbox.h"
#include "pic.h"
#include "cgascr.h"
#include "key.h"
#include "keyctrl.h"
#include <stdbool.h>

Key key = {0, 0, 0};

void epilogue()
{
    pic_forbid(pic_keyboard);
    Key copy = key;
    pic_allow(pic_keyboard);
    CGA_putchar(copy.ascii);
}

bool prologue()
{
    key = keyctrl_key_hit();
    if (!key_valid(key))
        return false;

    if (key_ctrl(key) && key_alt(key) && key.scancode == key_del)
        keyctrl_reboot();

    return true;
}

void ps2kbd_plugin()
{
    interrupt_handler handler = {prologue, epilogue};
    plugbox_assign(int_keyboard, handler);
    pic_allow(pic_keyboard);
}
