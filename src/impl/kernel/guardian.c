#include "plugbox.h"
#include "guard.h"

extern void guardian(unsigned int slot, unsigned int *error_code);

void guardian(unsigned int slot, unsigned int *error_code)
{
    interrupt_handler *gate = plugbox_report(slot);
    if(gate->prologue())
        guard_relay(gate);
}
