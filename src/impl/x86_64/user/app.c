#include "user/app.h"
#include "guard.h"
#include "cgascr.h"
#include "stdlib/stdio.h"

void action()
{
    for(int i = 0;;i=(i%100)+1)
    {
        {
            guard_enter();
            CGA_setpos(0, 2);
            printf("app1: %c", i);
            guard_leave();
        }
    }
}
void action2()
{
    for(int i = 0;;i=(i%100)+1)
    {
        {
            guard_enter();
            CGA_setpos(2, 4);
            printf("app2: %c", i);
            guard_leave();
        }
    }
}

Coroutine a1;
Coroutine a2;

Coroutine *app(void *tos)
{
    a1 = new_coroutine(action);
    coroutine_init(&a1, tos);
    return &a1;
}
Coroutine *app2(void *tos)
{
    a2 = new_coroutine(action2);
    coroutine_init(&a2, tos);
    return &a2;
}
