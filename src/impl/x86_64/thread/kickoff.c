#include "thread/coroutine.h"
#include "guard.h"
#include "panic.h"

void kickoff(void *a, void *b, void *c, void *d, void *e, void *f, Coroutine* coroutine)
{
    guard_leave();
    coroutine->action();
    panic("kickoff terminated");
}
