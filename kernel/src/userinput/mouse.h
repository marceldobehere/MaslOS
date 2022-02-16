#pragma once
#include  "../IO.h"

void Mousewait()
{
    uint64_t timeout = 100000;
    while (timeout--)
        if ((inb(0x64) & 0b10) == 0)
            return;

}

void MousewaitInput()
{
    uint64_t timeout = 100000;
    while (timeout--)
        if (inb(0x64) & 0b1)
            return;

}

void MouseWrite(uint8_t value)
{
    Mousewait();
    outb(0x64, 0xD4);
    Mousewait();
    outb(0x60, value);
}

uint8_t MouseRead()
{
    MousewaitInput();
    return inb(0x60);
}

void InitPS2Mouse()
{
    outb(0x64, 0xA8);
    Mousewait();
    outb(0x64, 0x20);
    MousewaitInput();

    uint8_t status = inb(0x60);
    status |= 0b10;
    Mousewait();
    outb(0x64, 0x60);
    Mousewait();
    outb(0x60, status);

    MouseWrite(0xF6);
    MouseRead();

    MouseWrite(0xF4);
    MouseRead();

}


void HandlePS2Mouse(uint8_t data)
{

}