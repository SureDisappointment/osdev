#pragma once

#ifndef NDEBUG
void panic(const char* message);
#define assert(x, m)  \
    do                \
    {                 \
        if (!(x))     \
            panic(m); \
    } while (0)
#else
#define assert(x, m) ((void)0)
#endif
