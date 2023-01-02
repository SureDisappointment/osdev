#pragma once
#define SYSV __attribute__((sysv_abi))

#include <stddef.h>
#include <stdarg.h>

// TODO: uppercase hex, floats, length modifiers, keep track of amount printed in printf()
SYSV int printf(const char *fmt, ...);
SYSV int vprintf(const char *fmt, va_list args);
/*
SYSV int sprintf(char *buffer, const char *fmt, ...);
SYSV int snprintf(char *buffer, size_t bufsz, const char *fmt, ...);
*/
