#include "cgascr.h"
#include "stdlib/stdio.h"
#include "io_port.h"

void kmain()
{
    outb(0x21,0xfd);
    outb(0xa1,0xff);
    asm("sti");

    CGA_clear();
    CGA_set_color(CGA_F_YELLOW | CGA_B_BLACK);
    CGA_puts("Goodbye, World!\n");
    printf("--- %i %i %o %x %i %i %i %i %u ---\n", 3, 6, 9, 10, 12, 13, 14, 15, -16);
    printf("--- %c %s %s %s ---\n", 'x', "ABC", "DEFG", "Z");

    for(;;)
    {
        asm("hlt");
    }
}
