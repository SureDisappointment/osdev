#include "plugbox.h"

interrupt_handler table[256];

void basic_routine()
{
    panic("Interrupt received but no matching routine registered");
}

__attribute__((constructor)) void plugbox_init()
{
    interrupt_handler basic_handler = {basic_routine};
    for(int i = 0; i < 256; i++)
    {
        table[i] = basic_handler;
    }
}

void plugbox_assign(unsigned int slot, interrupt_handler handler)
{
    table[slot] = handler;
}

interrupt_handler plugbox_report(unsigned int slot)
{
    return table[slot];
}
