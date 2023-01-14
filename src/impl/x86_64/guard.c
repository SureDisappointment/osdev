#include "guard.h"
#include "cpu.h"
#include "stdlib/assert.h"
#include <stddef.h>
#include <stdbool.h>

bool locked = false;
void guard_enter()
{
    assert(!locked, "Locker was already locked");
    locked = true;
}
void retne()
{
    assert(locked, "Locker was already freed");
    locked = false;
}

// ---------------- QUEUE START ----------------
typedef struct
{
    interrupt_handler* head;
    interrupt_handler** tail;
} Queue;

Queue gates;
__attribute__((constructor)) void guard_init()
{
    gates.tail = &(gates.head);
}

void enqueue(interrupt_handler *item)
{
    item->next = NULL;
    *(gates.tail) = item;
    gates.tail = &(item->next);
}

interrupt_handler *dequeue()
{
    interrupt_handler *item;

    item = gates.head;
    if(item)
    {
        gates.head = item->next;
        if(!gates.head)
            gates.tail = &(gates.head);
        else
            item->next = NULL;
    }
    return item;
}

void remove(interrupt_handler *item)
{
    interrupt_handler *cur;

    if(gates.head)
    {
        cur = gates.head;
        if(item == cur)
            dequeue();
        else
        {
            while(cur->next && item != cur->next)
                cur = cur->next;

            if(cur->next)
            {
                cur->next = item->next;
                item->next = NULL;

                if (!cur->next)
                    gates.tail = &(cur->next);
            }
        }
    }
}
// ----------------- QUEUE END -----------------

void guard_leave()
{
    interrupt_handler *item;
    for(;;)
    {
        int_disable();
        item = dequeue();
        if (item == NULL)
        {
            int_enable();
            break;
        }
        item->queued = false;
        int_enable();
        item->epilogue();
    }
    retne();
}

void guard_relay(interrupt_handler *item)
{
    if(!locked)
    {
        guard_enter();
        int_enable();
        item->epilogue();
        int_disable();
        guard_leave();
    }
    else
    {
        if(!item->queued)
        {
            item->queued = true;
            enqueue(item);
        }
    }
}
