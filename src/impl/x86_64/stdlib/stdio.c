#include "stdlib/stdio.h"
#include "stdlib/string.h"
#include "stdlib/algorithm.h"
#include <stdint.h>
#include <stdarg.h>
#include "cgascr.h"

enum Printf_State
{
    PRINTF_STATE_NORMAL = 0,
    PRINTF_STATE_LENGTH = 1,
    PRINTF_STATE_LENGTH_SHORT = 2,
    PRINTF_STATE_LENGTH_LONG = 3,
    PRINTF_STATE_SPEC = 4,
};

enum Printf_Length
{
    PRINTF_LENGTH_DEFAULT = 0,
    PRINTF_LENGTH_SHORT_SHORT = 1,
    PRINTF_LENGTH_SHORT = 2,
    PRINTF_LENGTH_LONG = 3,
    PRINTF_LENGTH_LONG_LONG = 4,
};

SYSV int printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

SYSV int vprintf(const char *fmt, va_list args)
{
    enum Printf_State state = PRINTF_STATE_NORMAL;
    enum Printf_Length length = PRINTF_LENGTH_DEFAULT;

    while(*fmt)
    {
        switch(state)
        {
            case PRINTF_STATE_NORMAL:
                switch(*fmt)
                {
                    case '%':
                        state = PRINTF_STATE_LENGTH;
                        break;
                    default:
                        CGA_putchar(*fmt);
                        break;  
                }
                break;
            
            case PRINTF_STATE_LENGTH:
                switch(*fmt)
                {
                    case 'h':
                        length = PRINTF_LENGTH_SHORT;
                        state = PRINTF_STATE_LENGTH_SHORT;
                        break;
                    case 'l':
                        length = PRINTF_LENGTH_LONG;
                        state = PRINTF_STATE_LENGTH_LONG;
                        break;
                    default:
                        goto PRINTF_STATE_SPEC_;
                }
                break;
            
            case PRINTF_STATE_LENGTH_SHORT:
                if(*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;
            
            case PRINTF_STATE_LENGTH_LONG:
                if(*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;
            
            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch(*fmt)
                {
                    char buf[23];
                    case 'c':
                        CGA_putchar((char) va_arg(args, int));
                        break;
                    case 's':
                        CGA_puts(va_arg(args, const char*));
                        break;
                    case '%':
                        CGA_putchar('%');
                        break;
                    case 'd':
                    case 'i':
                        CGA_puts(itoa(va_arg(args, int64_t), buf, 10));
                        break;
                    case 'u':
                        CGA_puts(uitoa(va_arg(args, uint64_t), buf, 10));
                        break;
                    case 'p':
                        CGA_puts("0x");
                    case 'X':
                        // TODO: uppercase hex
                    case 'x':
                        CGA_puts(itoa(va_arg(args, int64_t), buf, 16));
                        break;
                    case 'o':
                        CGA_puts(itoa(va_arg(args, int64_t), buf, 8));
                        break;
                    case 'f':
                    case 'F':
                        // TODO:
                        break;
                    case 'e':
                    case 'E':
                        // TODO:
                        break;
                    case 'a':
                    case 'A':
                        // TODO:
                        break;
                    case 'g':
                    case 'G':
                        // TODO:
                        break;
                    case 'n':
                        // TODO: keep track of length
                        //*argp++ = ;
                        break;
                    default:
                        break;
                }

                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                break;
        }
        fmt++;
    }

    // TODO: keep track of length
    return 0;
}
/*
SYSV int sprintf(char *buffer, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
}

SYSV int vsprintf(char *buffer, const char *fmt, va_list args)
{
    char *start = buffer;
    
    enum Printf_State state = PRINTF_STATE_NORMAL;
    enum Printf_Length length = PRINTF_LENGTH_DEFAULT;

    while(*fmt)
    {
        switch(state)
        {
            case PRINTF_STATE_NORMAL:
                switch(*fmt)
                {
                    case '%':
                        state = PRINTF_STATE_LENGTH;
                        break;
                    default:
                        *buffer++ = *fmt;
                        break;  
                }
                break;
            
            case PRINTF_STATE_LENGTH:
                switch(*fmt)
                {
                    case 'h':
                        length = PRINTF_LENGTH_SHORT;
                        state = PRINTF_STATE_LENGTH_SHORT;
                        break;
                    case 'l':
                        length = PRINTF_LENGTH_LONG;
                        state = PRINTF_STATE_LENGTH_LONG;
                        break;
                    default:
                        goto PRINTF_STATE_SPEC_;
                }
                break;
            
            case PRINTF_STATE_LENGTH_SHORT:
                if(*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;
            
            case PRINTF_STATE_LENGTH_LONG:
                if(*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;
            
            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch(*fmt)
                {
                    case 'c':
                        *buffer++ = (char) va_arg(args, int);
                        break;
                    case 's':
                        *buffer = '\0';
                        strcat(buffer, va_arg(args, const char*));
                        buffer += strlen(buffer);
                        break;
                    case '%':
                        *buffer++ = '%';
                        break;
                    case 'd':
                    case 'i':
                        itoa(va_arg(args, int64_t), buffer, 10);
                        buffer += n_digits(*argp, 10) + (*argp < 0 ? 1 : 0);
                        argp++;
                        break;
                    case 'u':
                        uitoa(*(uint64_t *)argp, buffer, 10);
                        buffer += n_digits(*argp, 10);
                        argp++;
                        break;
                    case 'p':
                        *buffer++ = '0';
                        *buffer++ = 'x';
                    case 'X':
                        // TODO: uppercase hex
                    case 'x':
                        itoa(*argp, buffer, 16);
                        buffer += n_digits(*argp, 16);
                        argp++;
                        break;
                    case 'o':
                        itoa(*argp, buffer, 8);
                        buffer += n_digits(*argp, 8);
                        argp++;
                        break;
                    case 'f':
                    case 'F':
                        // TODO:
                        break;
                    case 'e':
                    case 'E':
                        // TODO:
                        break;
                    case 'a':
                    case 'A':
                        // TODO:
                        break;
                    case 'g':
                    case 'G':
                        // TODO:
                        break;
                    case 'n':
                        // TODO: keep track of length
                        *argp++ = buffer - start;
                        break;
                    default:
                        break;
                }

                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                break;
        }
        fmt++;
    }

    *buffer = '\0';
    return buffer - start;
}

SYSV int snprintf(char *buffer, size_t bufsz, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, bufsz, fmt, args);
    va_end(args);
}

SYSV int vsnprintf(char *buffer, size_t bufsz, const char *fmt, va_list args)
{
    char *start = buffer;
    
    enum Printf_State state = PRINTF_STATE_NORMAL;
    enum Printf_Length length = PRINTF_LENGTH_DEFAULT;

    int64_t args[3];
    asm volatile("mov %%rcx, %0" : "=m" (args[0]));
    asm volatile("mov %%r8, %0" : "=m" (args[1]));
    asm volatile("mov %%r9, %0" : "=m" (args[2]));
    int64_t *argp = args;
    int on_stack = 0;

    while(*fmt)
    {
        if(buffer - start == bufsz)
        {
            *buffer = '\0';
            return bufsz;
        }
        if(argp - args == 3 && !on_stack)
        {
            asm("mov %%rbp, %0" : "=r" (argp));
            argp += 2;
            on_stack = 1;
        }
        switch(state)
        {
            case PRINTF_STATE_NORMAL:
                switch(*fmt)
                {
                    case '%':
                        state = PRINTF_STATE_LENGTH;
                        break;
                    default:
                        *buffer++ = *fmt;
                        break;  
                }
                break;
            
            case PRINTF_STATE_LENGTH:
                switch(*fmt)
                {
                    case 'h':
                        length = PRINTF_LENGTH_SHORT;
                        state = PRINTF_STATE_LENGTH_SHORT;
                        break;
                    case 'l':
                        length = PRINTF_LENGTH_LONG;
                        state = PRINTF_STATE_LENGTH_LONG;
                        break;
                    default:
                        goto PRINTF_STATE_SPEC_;
                }
                break;
            
            case PRINTF_STATE_LENGTH_SHORT:
                if(*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;
            
            case PRINTF_STATE_LENGTH_LONG:
                if(*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;
            
            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch(*fmt)
                {
                    char buf[23];
                    size_t len;
                    size_t space_left;
                    char *s;
                    case 'c':
                        *buffer++ = *(char *)argp;
                        argp++;
                        break;
                    case 's':
                        s = *(char **)argp;
                        len = strlen(s);
                        space_left = bufsz - (buffer - start);
                        if(len >= space_left)
                            len = space_left - 1;
                        memmove(buffer, s, len);
                        buffer += len;
                        argp++;
                        break;
                    case '%':
                        *buffer++ = '%';
                        break;
                    case 'd':
                    case 'i':
                        itoa(*argp, buf, 10);
                        len = strlen(buf);
                        space_left = bufsz - (buffer - start);
                        if(len >= space_left)
                            len = space_left - 1;
                        memmove(buffer, buf, len);
                        buffer += len;
                        argp++;
                        break;
                    case 'u':
                        uitoa(*(uint64_t *)argp, buf, 10);
                        len = strlen(buf);
                        space_left = bufsz - (buffer - start);
                        if(len >= space_left)
                            len = space_left - 1;
                        memmove(buffer, buf, len);
                        buffer += len;
                        argp++;
                    case 'p':
                        *buffer++ = '0';
                        if(buffer - start == bufsz)
                        {
                            *buffer = '\0';
                            return bufsz;
                        }
                        *buffer++ = 'x';
                        if(buffer - start == bufsz)
                        {
                            *buffer = '\0';
                            return bufsz;
                        }
                    case 'X':
                        // TODO: uppercase hex
                    case 'x':
                        itoa(*argp, buf, 16);
                        len = strlen(buf);
                        space_left = bufsz - (buffer - start);
                        if(len >= space_left)
                            len = space_left - 1;
                        memmove(buffer, buf, len);
                        buffer += len;
                        argp++;
                        break;
                    case 'o':
                        itoa(*argp, buf, 8);
                        len = strlen(buf);
                        space_left = bufsz - (buffer - start);
                        if(len >= space_left)
                            len = space_left - 1;
                        memmove(buffer, buf, len);
                        buffer += len;
                        argp++;
                        break;
                    case 'f':
                    case 'F':
                        // TODO:
                        break;
                    case 'e':
                    case 'E':
                        // TODO:
                        break;
                    case 'a':
                    case 'A':
                        // TODO:
                        break;
                    case 'g':
                    case 'G':
                        // TODO:
                        break;
                    case 'n':
                        *argp++ = buffer - start;
                        break;
                    default:
                        break;
                }

                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                break;
        }
        fmt++;
    }

    *buffer = '\0';
    return buffer - start;
}
*/
