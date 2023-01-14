#include "thread/coroutine.h"
#include <stddef.h>

extern void toc_settle(/*OUT*/ toc* regs, void* tos, kickoff_func kickoff, void* coroutine);
extern void toc_go(/*IN*/ toc* regs);
extern void toc_switch(/*OUT*/ toc* regs_now, /*IN*/ toc* reg_then);

void kickoff(void*, void*, void*, void*, void*, void*, Coroutine* coroutine);

Coroutine new_coroutine(void (*action)())
{
    Coroutine c = {action, new_toc(), NULL};
    return c;
}
void coroutine_init(Coroutine *c, void* tos)
{
    toc_settle(&(c->mtoc), tos, (kickoff_func)(&kickoff), c);
}
void coroutine_go(Coroutine *c)
{
    toc_go(&(c->mtoc));
}
void coroutine_resume(Coroutine *c, Coroutine *next)
{
    toc_switch(&(c->mtoc), &(next->mtoc));
}
