#pragma once

#include <stddef.h>

void swap(void *a, void *b);

void *memset(void *s, int c, size_t n);

void *memcpy(void *dest, const void *src, size_t n);

void *memmove(void *dest, const void *src, size_t n);
