#pragma once

#include "panic.h"

extern void guardian(unsigned int slot);

void guardian(unsigned int slot)
{
    panicf("Interrupt received: %u", slot);
}
