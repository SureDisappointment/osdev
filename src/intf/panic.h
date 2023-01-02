#pragma once

void panicf(const char *fmt, ...);
inline void panic(const char msg[])
{
    panicf(msg);
}
