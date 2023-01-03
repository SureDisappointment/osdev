#pragma once

#include <stdbool.h>

typedef enum
{
    int_timer = 32,
    int_keyboard = 33,
} interrupt_number;

typedef struct
{
    bool (*prologue)();
    void (*epilogue)();
} interrupt_handler;


void plugbox_assign(interrupt_number slot, interrupt_handler handler);
interrupt_handler plugbox_report(unsigned int slot);
