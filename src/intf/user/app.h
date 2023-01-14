#pragma once

#include "thread/coroutine.h"

Coroutine *app(void *tos);
Coroutine *app2(void *tos);
void init();