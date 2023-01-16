#include "plugbox.h"
#include "guard.h"

extern void guardian(unsigned int slot, unsigned int *error_code);

void guardian(unsigned int slot, unsigned int *error_code)
{
    int e = 0;
    if(slot == 8
        || (slot >= 10 && slot <= 14)
        || slot == 17
        || slot == 21
        || slot == 29
        || slot == 30)
    {
        e = *error_code;
    }
    // TODO: pass error code to exceptions
    interrupt_handler *gate = plugbox_report(slot);
    if(gate->prologue())
        guard_relay(gate);
}
