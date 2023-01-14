#include "cpu.h"

inline void int_enable()
{
    asm("sti");
}
inline void int_disable()
{
    asm("cli");
}
inline void cpu_idle()
{
    asm("sti");
    asm("hlt");
}
inline void cpu_halt()
{
    asm("cli");
    asm("hlt");
}
