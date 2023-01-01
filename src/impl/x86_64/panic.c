#include "panic.h"
#include "cgascr.h"
#include "cpu.h"
#include "stdlib/string.h"
#include "stdlib/stdio.h"
#include <stdint.h>
#include <stddef.h>

void panic(const char msg[])
{
    CGA_clear();
    CGA_setpos(0, 0);

    CGA_set_color(CGA_F_LIGHT_RED);
    CGA_puts("Kernel panic\n");
    CGA_set_color(CGA_DEFAULT_COLOR);
    if (msg)
        printf("%s\n", msg);
    CGA_puts("Stacktrace:\n");

    // Print Stacktrace
    struct stack_frame
    {
        struct stack_frame *prev;
        void *return_addr;
    } __attribute__((packed));

    struct stack_frame *fp;
    asm volatile("movq %%rbp, %[fp]" : [fp] "=r"(fp));

    // Don't show more than 10 entries
    for (int i = 0; i < 10 && fp; fp = fp->prev, i++)
    {
        printf("\t%p\n", (uintptr_t)fp->return_addr);
    }

    cpu_halt();
}