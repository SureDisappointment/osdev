#pragma once

#include "machine/toc.h"

typedef struct Coroutine
{
    void (*action)();
    toc mtoc;
    struct Coroutine *next;
} Coroutine;

Coroutine new_coroutine(void (*action)());
void coroutine_init(Coroutine* c, void* tos);
void coroutine_go(Coroutine *c);
void coroutine_resume(Coroutine *c, Coroutine *next);
