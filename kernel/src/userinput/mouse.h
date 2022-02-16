#pragma once
#include  "../IO.h"

#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

#define PS2LeftButton   0b00000001
#define PS2MiddleButton 0b00000010
#define PS2RightButton  0b00000100

Point MousePosition;

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
    MousePosition.x = 0;
    MousePosition.y = 0;
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

uint8_t MouseCycle = 0;
uint8_t MousePacket[4];
bool MousePacketReady = 0;

void HandlePS2Mouse(uint8_t data)
{
    switch(MouseCycle)
    {
        case 0:
        {
            if(MousePacketReady)
                break;
            if (data &0b00001000 == 0)
                break;

            MousePacket[0] = data;
            MouseCycle++;
            break;
        }
        case 1:
        {
            MousePacket[1] = data;
            MouseCycle++;
            break;
        }
        case 2:
        {
            MousePacket[1] = data;
            MouseCycle++;
            MousePacketReady = true;
            MouseCycle = 0;
            break;
        }
    }
}

void ProcessMousePacket()
{
    if(!MousePacketReady)
        return;
    MousePacketReady = false;

    GlobalRenderer->Print("A");

    bool xNegative, yNegative, xOverflow, yOverflow, leftButton, middleButton, rightButton;

    if(MousePacket[0] & PS2XSign)
        xNegative = true;
    else
        xNegative = false;

    if(MousePacket[0] & PS2YSign)
        yNegative = true;
    else
        yNegative = false;

    if(MousePacket[0] & PS2XOverflow)
        xOverflow = true;
    else
        xOverflow = false;

    if(MousePacket[0] & PS2YOverflow)
        yOverflow = true;
    else
        yOverflow = false;
    
    if (!xNegative)
    {
        MousePosition.x += MousePacket[1];
        if (xOverflow)
            MousePosition.x += 255;
    }
    else
    {
        MousePacket[1] = 256 - MousePacket[1];
        MousePosition.x -= MousePacket[1];
        if (xOverflow)
            MousePosition.x -= 255;
    }

    if (!yNegative)
    {
        MousePacket[2] = 256 - MousePacket[2];
        MousePosition.y -= MousePacket[2];
        if (yOverflow)
            MousePosition.y -= 255;
    }
    else
    {
        MousePosition.y += MousePacket[2];
        if (yOverflow)
            MousePosition.y += 255;
    }

    if(MousePosition.x < 0)
        MousePosition.x = 0;
    else if(MousePosition.x > GlobalRenderer->framebuffer->Width - 8)
        MousePosition.x = GlobalRenderer->framebuffer->Width - 8;

    if(MousePosition.y < 0)
        MousePosition.y = 0;
    else if(MousePosition.y > GlobalRenderer->framebuffer->Height - 16)
        MousePosition.y = GlobalRenderer->framebuffer->Height - 16;

    GlobalRenderer->putChar('A', MousePosition.x, MousePosition.y);    


    if(MousePacket[0] & PS2LeftButton)
        leftButton = true;
    else
        leftButton = false;

    if(MousePacket[0] & PS2RightButton)
        rightButton = true;
    else
        rightButton = false;

    if(MousePacket[0] & PS2MiddleButton)
        middleButton = true;
    else
        middleButton = false;


}