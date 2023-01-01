#include "stdlib/string.h"
#include "stdlib/algorithm.h"
#include <stdint.h>

void reverse_str(char *str, int length)
{
    int start = 0;
    int end = length-1;
    while(start < end)
    {
        swap(&str[start], &str[end], sizeof(char));
        start++;
        end--;
    }
}

int n_digits(int64_t n, int base)
{
    if(n == 0)
        return 1;
    int count = 0;
    while (n != 0) {
        count++;
        n /= base;
    }
    return count;
}

char *itoa(int64_t n, char str[], int base)
{
    int i = 0;
    int neg = 0;

    if (n == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    if (n < 0 && base == 10)
    {
        neg = 1;
        n = -n;
    }

    while (n != 0)
    {
        int rem = n % base;
        str[i++] = (rem > 9) ? (rem-10) + 'a' : rem + '0';
        n = n / base;
    }

    if (neg)
        str[i++] = '-';

    str[i] = '\0';

    reverse_str(str, i);
    return str;
}

char *uitoa(uint64_t n, char str[], int base)
{
    int i = 0;

    if (n == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    while (n != 0)
    {
        int rem = n % base;
        str[i++] = (rem > 9) ? (rem-10) + 'a' : rem + '0';
        n = n / base;
    }

    str[i] = '\0';

    reverse_str(str, i);
    return str;
}

char *strcat(char *dest, const char *src)
{
    char *ret = dest;
    while (*dest)
        dest++;
    while (*dest++ = *src++)
        ;
    return ret;
}

size_t strlen(const char *s)
{
    const char *p = s;
    while (*s) ++s;
    return s - p;
}
