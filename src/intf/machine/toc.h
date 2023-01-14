#pragma once

typedef struct
{
    void* rbx;
    void* r12;
    void* r13;
    void* r14;
    void* r15;
    void* rbp;
    void* rsp;
    char fpu[108]; // Optional: 108 Byte extended CPU state (MMX, SSE, ...)
} toc;

toc new_toc();
typedef void (*kickoff_func)(void*, void*, void*, void*, void*, void*, void*);
