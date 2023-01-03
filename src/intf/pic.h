#pragma once

typedef enum
{
    pic_timer = 0,
    pic_keyboard = 1,
    pic_com2 = 2,
    pic_com1 = 4,

    pic_floppy = 6,
    pic_lpt1 = 7,
    pic_cmos_echtzeituhr = 8,
    pic_hwm_irq2 = 9,

    pic_ps2 = 12,
    pic_num_coproc = 13,
    pic_ide1 = 14,
    pic_ide2 = 15
} pic_number;

void pic_allow(pic_number device);
void pic_forbid(pic_number device);
int pic_ismasked(pic_number device);
