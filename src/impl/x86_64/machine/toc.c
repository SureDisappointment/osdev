#include "machine/toc.h"
#include <stdint.h>
#include <stddef.h>

void toc_settle(/*OUT*/ toc* regs, void* tos, kickoff_func kickoff, void* coroutine)
{
    regs->rbp = (void*)((uintptr_t*)tos - 1);
    regs->rsp = (void*)((uintptr_t*)tos - 3);
    ((uintptr_t*)regs->rsp)[2] = (uintptr_t)coroutine;
    ((kickoff_func*)regs->rsp)[0] = kickoff;
}

toc new_toc()
{
    toc t = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, {0}};
    return t;
}
