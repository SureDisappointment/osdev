#pragma once
#define SYSV __attribute__((sysv_abi))

#include <stddef.h>

// TODO: uppercase hex, floats, length modifiers, keep track of amount printed in printf()
SYSV int printf(const char *fmt, ...);
SYSV int sprintf(char *buffer, const char *fmt, ...);
SYSV int snprintf(char *buffer, size_t bufsz, const char *fmt, ...);
