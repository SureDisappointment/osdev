#pragma once

#include "thread/coroutine.h"

void scheduler_ready(Coroutine *that);
void scheduler_schedule();
void scheduler_exit();
void scheduler_kill(Coroutine *that);
void scheduler_resume();
