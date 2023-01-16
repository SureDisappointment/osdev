#include "device/watch.h"
#include "plugbox.h"
#include "pic.h"
#include "thread/scheduler.h"
#include "io_port.h"
#include "stdlib/assert.h"
#include <stddef.h>

unsigned int us_interval = 0;
unsigned int it = 0;
unsigned int it_max = 0;

bool pit_prologue()
{
    if(it >= it_max)
    {
        it = 0;
        return true;
    }
    it++;
    return false;
}

void watch_epilogue(){};

void watch_plugin(void (*epilogue)())
{
    if(epilogue == NULL)
    {
        epilogue = watch_epilogue;
    }
    plugbox_assign(int_timer, new_interrupt_handler(pit_prologue, epilogue));
    pic_allow(pic_timer);
}

void pit_interval(int us)
{
    assert(us >= 1000, "Timer cannot be faster than 1 ms");
    assert(us <= 60000, "Timer cannot be slower than 60 ms");
    unsigned char high = (us>>8) & 0xFF;
    unsigned char low = us & 0xFF;
    outb(0x43, 0x34); // 00 11 010 0 -> first low, then high byte into counter 0; periodic interrupts
    outb(0x40, low);
    outb(0x40, high);

    us_interval = us;
}

void watch_set(unsigned int us, unsigned int iterations)
{
    if(us != us_interval)
        pit_interval(us);
    us_interval = us;
    it_max = iterations;
}
