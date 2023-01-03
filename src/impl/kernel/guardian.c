#pragma once

#include "plugbox.h"

extern void guardian(unsigned int slot);

void guardian(unsigned int slot)
{
    interrupt_handler gate = plugbox_report(slot);
    if(gate.prologue())
        gate.epilogue();
}
