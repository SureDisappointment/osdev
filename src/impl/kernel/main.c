#include "cgascr.h"

void kmain() {
    CGA_clear();
    CGA_set_color(CGA_F_YELLOW | CGA_B_BLACK);
    CGA_puts("Hello, Bozo!");
}
