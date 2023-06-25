#include "IO.h"

#define DEF_IO_WAIT 100

void _outb(uint16_t port, uint8_t value)
{
    //asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
    __asm__ __volatile__("outb %1, %0" : : "dN"(port), "a"(value));
}
void outb(uint16_t port, uint8_t value)
{
    _outb(port, value);
    io_wait(DEF_IO_WAIT);
}

uint8_t _inb(uint16_t port)
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

uint8_t inb(uint16_t port)
{
    io_wait(DEF_IO_WAIT);
    return _inb(port);
}

void outw(uint16_t port, uint16_t value)
{
    __asm__ volatile("outw %w0, %w1" : : "a" (value), "Nd" (port));
    io_wait(DEF_IO_WAIT);
}

uint16_t inw(uint16_t port)
{
    uint16_t value;
    __asm__ volatile("inw %w1, %w0" : "=a" (value) : "Nd" (port));
    io_wait(DEF_IO_WAIT);
    return value;
}

void outl(uint16_t port, uint32_t value)
{
    __asm__ volatile("outl %0, %1" : : "a" (value), "Nd" (port));
    io_wait(DEF_IO_WAIT);
}

uint32_t inl(uint16_t port)
{
    uint32_t value;
    __asm__ volatile("inl %1, %0" : "=a" (value) : "Nd" (port));
    io_wait(DEF_IO_WAIT);
    return value;
}

void io_wait()
{
    _inb(0x80);
    _outb(0x80, 0);
    //asm volatile ("outb %%al, $0x80" : : "a"(0));
} 

void io_wait(uint64_t us) 
{
    while(us--)
        io_wait();
}