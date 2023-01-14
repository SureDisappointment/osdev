#include "plugbox.h"
#include "panic.h"
#include "exception.h"
#include <stddef.h>

interrupt_handler table[256];

bool basic_prologue()
{
    panic("Interrupt received but no matching routine registered");
    return false;
}

interrupt_handler new_interrupt_handler(bool (*prologue)(), void (*epilogue)())
{
    interrupt_handler h = {prologue, epilogue, false, NULL};
    return h;
}

__attribute__((constructor)) void plugbox_init()
{
    exception_defaults();
    for(int i = 32; i < 256; i++)
    {
        table[i] = new_interrupt_handler(basic_prologue, NULL);;
    }
}

void plugbox_assign(unsigned int slot, interrupt_handler handler)
{
    table[slot] = handler;
}

interrupt_handler *plugbox_report(unsigned int slot)
{
    return &table[slot];
}
