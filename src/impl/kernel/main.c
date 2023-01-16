#include "panic.h"
#include "cgascr.h"
#include "stdlib/stdio.h"
#include "io_port.h"
#include "device/ps2_keyboard.h"
#include "guard.h"
#include "cpu.h"
#include "user/app.h"
#include "thread/scheduler.h"
#include "device/watch.h"
#include <stdint.h>

extern void set_kernel_stack(uint64_t stackptr);
extern void jump_usermode(uint64_t start_addr);

void test_user_function()
{
    CGA_show(0, 0, 'U');
    for(;;);
}

void kmain()
{
    // for(;;)
    //     asm("hlt");

    uint64_t rsp;
    asm volatile("mov %%rsp, %0" : "=r"(rsp));
    set_kernel_stack(rsp);
    guard_enter();

    CGA_clear();

    ps2kbd_plugin();
    int_enable();

    jump_usermode((uint64_t)test_user_function);

    // temporary stacks for testing context switches
    unsigned char stack1[(1 << 10)];
    void* stack1_bp = stack1 + sizeof(stack1);

    unsigned char stack2[(1 << 10)];
    void* stack2_bp = stack2 + sizeof(stack2);

    Coroutine *c1 = app(stack1_bp);
    Coroutine *c2 = app2(stack2_bp);
    scheduler_ready(c1);
    scheduler_ready(c2);
    watch_set(60000, 20);
    watch_plugin(scheduler_resume);
    scheduler_schedule();

    /*CGA_clear();
    CGA_set_color(CGA_F_YELLOW | CGA_B_BLACK);
    CGA_puts("Goodbye, World!\n");
    printf("--- %i %i %o %x %i %i %i %i %u ---\n", 3, 6, 9, 10, 12, 13, 14, 15, -16);
    printf("--- %c %s %s %s ---\n", 'x', "ABC", "DEFG", "Z");*/

    for(;;)
        asm("hlt");
    guard_leave();
}
