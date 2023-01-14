#pragma once

#include "plugbox.h"

void guard_enter();

void guard_leave();
void guard_relay(interrupt_handler *item);
