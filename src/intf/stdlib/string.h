#pragma once

#include <stddef.h>
#include <stdint.h>

void reverse_str(char str[], int length);
int n_digits(int64_t n, int base);
char *itoa(int64_t n, char str[], int base);
char *uitoa(uint64_t n, char str[], int base);
char *strcat(char *dest, const char *src);
size_t strlen(const char *s);
