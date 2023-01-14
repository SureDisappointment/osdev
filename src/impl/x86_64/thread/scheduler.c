#include "thread/scheduler.h"
#include "panic.h"
#include <stddef.h>
#include "panic.h"
#include "guard.h"

Coroutine *active = NULL;

void go(Coroutine *first)
{
    active = first;
    coroutine_go(active);
}
void dispatch(Coroutine *next)
{
    Coroutine *current = active;
    active = next;
    coroutine_resume(current, next);
}

// ---------------- QUEUE START ----------------
typedef struct
{
    Coroutine* head;
    Coroutine** tail;
} Queue;

Queue ready_list = {NULL, NULL};
__attribute__((constructor)) void scheduler_init()
{
    ready_list.tail = &(ready_list.head);
}

void scheduler_enqueue(Coroutine *item)
{
    item->next = NULL;
    *(ready_list.tail) = item;
    ready_list.tail = &(item->next);
}

Coroutine *scheduler_dequeue()
{
    Coroutine *item;

    item = ready_list.head;
    if(item)
    {
        ready_list.head = item->next;
        if(!ready_list.head)
            ready_list.tail = &(ready_list.head);
        else
            item->next = NULL;
    }
    return item;
}

void scheduler_remove(Coroutine *item)
{
    Coroutine *cur;

    if(ready_list.head)
    {
        cur = ready_list.head;
        if(item == cur)
            scheduler_dequeue();
        else
        {
            while(cur->next && item != cur->next)
                cur = cur->next;

            if(cur->next)
            {
                cur->next = item->next;
                item->next = NULL;

                if (!cur->next)
                    ready_list.tail = &(cur->next);
            }
        }
    }
}
// ----------------- QUEUE END -----------------

void scheduler_ready(Coroutine *that)
{
    scheduler_enqueue(that);
}

void scheduler_schedule()
{
    Coroutine *process = scheduler_dequeue();
    if(process)
        go(process);
}

void scheduler_exit()
{
    Coroutine *process = scheduler_dequeue();
    if(process)
        dispatch(process);
    else
        panic("Last coroutine exited");
}

void scheduler_kill(Coroutine *that)
{
    scheduler_remove(that);
}

void scheduler_resume()
{
    scheduler_enqueue(active);
    Coroutine *process = scheduler_dequeue();
    dispatch(process);
}
