#include "IO.h"

#define DEF_IO_WAIT 0

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


void mOutb(uint64_t address, uint8_t value)
{
    (*((volatile uint8_t*)(address)))=(value);
}
uint8_t mInb(uint64_t address)
{
    return (*((volatile uint8_t*)(address)));
}

void mOutw(uint64_t address, uint16_t value)
{
    (*((volatile uint16_t*)(address)))=(value);
}
uint16_t mInw(uint64_t address)
{
    return (*((volatile uint16_t*)(address)));
}

void mOutl(uint64_t address, uint32_t value)
{
    (*((volatile uint32_t*)(address)))=(value);
}
uint32_t mInl(uint64_t address)
{
    return (*((volatile uint32_t*)(address)));
}

void mOutq(uint64_t address, uint64_t value)
{
    (*((volatile uint64_t*)(address)))=(value);
}
uint64_t mInq(uint64_t address)
{
    return (*((volatile uint64_t*)(address)));
}