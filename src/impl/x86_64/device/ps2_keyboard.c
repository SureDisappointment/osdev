#include "device/ps2_keyboard.h"
#include "plugbox.h"
#include "pic.h"
#include "cgascr.h"

void interrupt_routine()
{
    CGA_puts("test");
}

void ps2kbd_plugin()
{
    interrupt_handler handler = {interrupt_routine};
    plugbox_assign(int_keyboard, handler);
    pic_allow(pic_keyboard);
}
