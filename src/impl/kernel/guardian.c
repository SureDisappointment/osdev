#pragma once

#include "plugbox.h"

extern void guardian(unsigned int slot);

void guardian(unsigned int slot)
{
    plugbox_report(slot).routine();
}
