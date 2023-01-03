#pragma once

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
extern void cpu_halt()
{
    asm("cli");
    asm("hlt");
}
