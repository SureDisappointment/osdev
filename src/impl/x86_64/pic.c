#include "pic.h"
#include "io_port.h"
#include "stdint.h"

uint16_t read_imr()
{
    uint8_t high = inb(0xA1);
    uint8_t low = inb(0x21);

    return (high << 8) | low;
}

void write_imr(uint16_t imr)
{
    uint8_t high = (imr >> 8) & 0xFF;
    uint8_t low = imr & 0xFF;

    outb(0xA1, high);
    outb(0x21, low);
}

void pic_allow(pic_number device)
{
    uint16_t imr = read_imr();
    uint16_t mask = 1 << device;
    imr &= ~mask;
    write_imr(imr);
}

void pic_forbid(pic_number device)
{
    uint16_t imr = read_imr();
    uint16_t mask = 1 << device;
    imr |= mask;
    write_imr(imr);
}

bool pic_ismasked(pic_number device)
{
    uint16_t imr = read_imr();
    return imr & (1 << device);
}
