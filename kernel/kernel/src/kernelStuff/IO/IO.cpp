#include "IO.h"

void outb(uint16_t port, uint8_t value)
{
    //asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
    __asm__ __volatile__("outb %1, %0" : : "dN"(port), "a"(value));
}

uint8_t inb(uint16_t port)
{
    // uint8_t returnVal;
    // asm volatile ("inb %1, %0"
    // : "=a"(returnVal)
    // : "Nd"(port));
    // return returnVal;

    uint8_t val;
    __asm__ __volatile__("inb %1, %0" : "=a"(val) : "dN"(port));
    return val;
}

void outw(uint16_t port, uint16_t value)
{
    __asm__ volatile("outw %w0, %w1" : : "a" (value), "Nd" (port));
}

uint16_t inw(uint16_t port)
{
    uint16_t value;
    __asm__ volatile("inw %w1, %w0" : "=a" (value) : "Nd" (port));
    return value;
}

void io_wait()
{
    outb(0x80, 0);
    //asm volatile ("outb %%al, $0x80" : : "a"(0));
} 