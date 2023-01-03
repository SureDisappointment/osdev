#include "device/ps2_keyboard.h"
#include "plugbox.h"
#include "pic.h"
#include "cgascr.h"
#include "key.h"
#include "keyctrl.h"

Key key = {0, 0, 0};

void interrupt_routine()
{
    key = keyctrl_key_hit();
    if (!key_valid(key))
        return;

    if (key_ctrl(key) && key_alt(key) && key.scancode == key_del)
        keyctrl_reboot();
    
    CGA_putchar(key.ascii);
}

void ps2kbd_plugin()
{
    interrupt_handler handler = {interrupt_routine};
    plugbox_assign(int_keyboard, handler);
    pic_allow(pic_keyboard);
}
