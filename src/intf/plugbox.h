#pragma once

#include <stdbool.h>

typedef enum
{
    // exceptions
    int_de = 0,
    int_db = 1,
    int_nmi = 2,
    int_bp = 3,
    int_of = 4,
    int_br = 5,
    int_ud = 6,
    int_nm = 7,
    int_df = 8,
    int_ts = 10,
    int_np = 11,
    int_ss = 12,
    int_gp = 13,
    int_pf = 14,
    int_mf = 16,
    int_ac = 17,
    int_mc = 18,
    int_xm = 19,
    int_xf = 19,
    int_ve = 20,
    int_cp = 21,
    int_hv = 28,
    int_vc = 29,
    int_sx = 30,

    // IRQs
    int_timer = 32,
    int_keyboard = 33,
} interrupt_number;

typedef struct interrupt_handler
{
    bool (*prologue)();
    void (*epilogue)();
    bool queued;
    struct interrupt_handler *next;
} interrupt_handler;

interrupt_handler new_interrupt_handler(bool (*prologue)(), void (*epilogue)());
void plugbox_assign(interrupt_number slot, interrupt_handler handler);
interrupt_handler *plugbox_report(unsigned int slot);
