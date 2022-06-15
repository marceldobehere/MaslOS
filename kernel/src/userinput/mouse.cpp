#include "mouse.h"
#include "../WindowStuff/Window/window.h"
#include "../WindowStuff/WindowManager/windowManager.h"
#include "../scheduling-pit/pit.h"

#define PS2XSign        0b00010000
#define PS2YSign        0b00100000
#define PS2XOverflow    0b01000000
#define PS2YOverflow    0b10000000

#define PS2LeftButton   0b00000001
#define PS2MiddleButton 0b00000100
#define PS2RightButton  0b00000010

#define MouseLeft   0
#define MouseRight  1 
#define MouseMiddle 2


uint32_t MouseDataMap[] =
{
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 14738687, 11714047, 11714047, 8163583, 8163583, 8163583, 4020991, 2508543, 2508543, 1851135, 1851135, 9983, 0, 0, 0, 
0, 11714047, 11714047, 8163583, 8163583, 4020991, 2508543, 2508543, 2508543, 2508543, 1851135, 9983, 0, 0, 0, 0,
0, 11714047, 8163583, 4020991, 4020991, 2508543, 2508543, 2508543, 1851135, 1851135, 1851135, 0, 0, 0, 0, 0,
0, 8163583, 8163583, 4020991, 2508543, 2508543, 2508543, 1851135, 1851135, 1851135, 0, 0, 0, 0, 0, 0,
0, 8163583, 4020991, 2508543, 2508543, 2508543, 1851135, 1851135, 1851135, 0, 0, 0, 0, 0, 0, 0,
0, 8163583, 2508543, 2508543, 2508543, 1851135, 1851135, 1851135, 470271, 470271, 0, 0, 0, 0, 0, 0,
0, 4020991, 2508543, 1851135, 1851135, 9983, 470271, 470007, 470007, 9197, 9197, 0, 0, 0, 0, 0,
0, 2508543, 2508543, 1851135, 470271, 470271, 470271, 470007, 470007, 9197, 9197, 9197, 0, 0, 0, 0,
0, 1851135, 1851135, 1851135, 470271, 0, 470007, 470007, 9197, 9197, 9197, 8408, 8408, 0, 0, 0,
0, 470271, 470271, 470271, 0, 0, 0, 9197, 9197, 9197, 8408, 8408, 8408, 6829, 0, 0,
0, 470271, 470271, 0, 0, 0, 0, 0, 9983, 8408, 8408, 8408, 6829, 6829, 4991, 0,
0, 470271, 0, 0, 0, 0, 0, 0, 0, 8408, 8408, 6829, 6829, 4991, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6829, 6829, 4991, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4991, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


uint8_t MouseShowBitmap[] =
{
    0b11111111, 0b11111110,
    0b11111111, 0b11111100,
    0b11111111, 0b11111000,
    0b11111111, 0b11110000,
    0b11111111, 0b11100000,
    0b11111111, 0b11000000,
    0b11111111, 0b11100000,
    0b11111111, 0b11110000,
    0b11111111, 0b11111000,
    0b11111111, 0b11111100,
    0b11111011, 0b11111110,
    0b11110001, 0b11111111,
    0b11100000, 0b11111110,
    0b11000000, 0b01111100,
    0b10000000, 0b00111000,
    0b00000000, 0b00010000
};

uint32_t MouseTempBitmap[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

extern uint32_t mouseColFront = Colors.white, mouseColBack = Colors.black;

MPoint oldMousePosition;

bool clicks[3] = {false, false, false};
double clickTimes[3] = {0, 0, 0};


MPoint MousePosition;

void SaveIntoBuffer(MPoint point, Framebuffer* framebuffer)
{
    unsigned int *pixPtr = (unsigned int*)framebuffer->BaseAddress;
    unsigned int pps = framebuffer->PixelsPerScanLine;

    unsigned long xoff = point.x;
    unsigned long yoff = point.y;

    unsigned long index = 0;

    for (unsigned long y = yoff; y < yoff + 16; y++)
        for (unsigned long x = xoff; x < xoff + 16; x++)
        {
            MouseTempBitmap[index] = *(unsigned int*)(pixPtr + x + (y * pps));
            index++;
        }
}

void LoadFromBuffer(MPoint point, Framebuffer* framebuffer)
{
    unsigned int *pixPtr = (unsigned int*)framebuffer->BaseAddress;
    unsigned int pps = framebuffer->PixelsPerScanLine;

    unsigned long xoff = point.x;
    unsigned long yoff = point.y;

    unsigned long index = 0;

    for (unsigned long y = yoff; y < yoff + 16; y++)
        for (unsigned long x = xoff; x < xoff + 16; x++)
        {
            *(unsigned int*)(pixPtr + x + (y * pps)) = MouseTempBitmap[index];
            index++;
        }
}

void DrawMouseBuffer(MPoint point, Framebuffer* framebuffer)
{
    unsigned int *pixPtr = (unsigned int*)framebuffer->BaseAddress;
    unsigned int pps = framebuffer->PixelsPerScanLine;

    unsigned long xoff = point.x;
    unsigned long yoff = point.y;

    unsigned long index = 0;
    unsigned long index_x = 0;
    unsigned long index_x2 = 0;
    for (unsigned long y = yoff; y < yoff + 16; y++)
    {
        index_x = 0;
        for (unsigned long x = xoff; x < xoff + 8; x++)
        {
            if((uint8_t)((uint8_t)(MouseShowBitmap[index] << index_x) >> 7) != 0)
                *(unsigned int*)(pixPtr + x + (y * pps)) = MouseDataMap[index_x2];
                
            index_x++;
            index_x2++;
        }
        index++;

        index_x = 0;
        for (unsigned long x = xoff + 8; x < xoff + 16; x++)
        {
            if((uint8_t)((uint8_t)(MouseShowBitmap[index] << index_x) >> 7) != 0)
                *(unsigned int*)(pixPtr + x + (y * pps)) = MouseDataMap[index_x2];
                
            index_x++;
            index_x2++;
        }
        index++;
    }

}

void DrawMousePointer()
{
    //DrawMousePointer1();
    //DrawMousePointer2();
}

void DrawMousePointer1(Framebuffer* framebuffer)
{
    LoadFromBuffer(oldMousePosition, framebuffer);
}

void DrawMousePointer2(Framebuffer* framebuffer)
{
    //SaveIntoBuffer(MousePosition, framebuffer);
    DrawMouseBuffer(MousePosition, framebuffer);
    oldMousePosition.x = MousePosition.x;
    oldMousePosition.y = MousePosition.y;
}



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

int mouseCycleSkip = 2;

MPoint diff = MPoint();
bool startDrag = false;
Window* dragWindow = NULL;

void InitPS2Mouse()
{
    mouseCycleSkip = 2;
    outb(0x64, 0xA8);
    Mousewait();

    for (int i = 0; i < 3; i++)
    {
        clicks[i] = false;
        clickTimes[i] = PIT::TimeSinceBoot;
    }

    diff = MPoint();
    startDrag = false;
    dragWindow = NULL;

    MousePosition.x = 50;
    MousePosition.y = 50;
    oldMousePosition.x = 50;
    oldMousePosition.y = 50;
    SaveIntoBuffer(MousePosition, GlobalRenderer->framebuffer);
    DrawMousePointer();


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
bool MousePacketReady = false;


void HandlePS2Mouse(uint8_t data)
{
    ProcessMousePacket();
    if (mouseCycleSkip != 0)
    {
        MouseCycle = mouseCycleSkip;
        mouseCycleSkip = 0;
        return;
    }

    switch(MouseCycle)
    {
        case 0:
        {
            // if(MousePacketReady)
            //     break;
            if (data & 0b00001000 == 0)
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
            MousePacket[2] = data;
            MousePacketReady = true;
            MouseCycle = 0;
            break;
        }
    }
}

void HandleClick(bool L, bool R, bool M)
{
    //activeWindow->renderer->Println("Click");
    if (L)
    {
        Window* window = WindowManager::getWindowAtMousePosition();
        activeWindow = window;
        dragWindow = window;
        startDrag = false;
        if (window != NULL)
        {
            diff.x = MousePosition.x - window->position.x;
            diff.y = MousePosition.y - window->position.y;
            window->moveToFront = true;
        }
    }   
}


void HandleHold(bool L, bool R, bool M)
{
    if (L)
    {
        Window* window = dragWindow;
        if (window != NULL)
        {
            if (!startDrag)
            {
                startDrag = true;
            }
            else
            {
                window->newPosition.x = MousePosition.x - diff.x;
                window->newPosition.y = MousePosition.y - diff.y;
            }
        }




    }
}





void ProcessMousePacket()
{
    if(!MousePacketReady)
        return;
    //MousePacketReady = false;

    //GlobalRenderer->Print("A");

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

    if (yNegative)
    {
        MousePacket[2] = 256 - MousePacket[2];
        MousePosition.y += MousePacket[2];
        if (yOverflow)
            MousePosition.y += 255;
    }
    else
    {
        //MousePacket[2] = 256 - MousePacket[2];
        MousePosition.y -= MousePacket[2];
        if (yOverflow)
            MousePosition.y -= 255;
    }

    //GlobalRenderer->overwrite = true;

    // GlobalRenderer->CursorPosition.x = 0;
    // GlobalRenderer->CursorPosition.y = 32;
    // GlobalRenderer->Println("X: {}        ", to_string((int64_t)MousePosition.x));
    // GlobalRenderer->Println("Y: {}        ", to_string((int64_t)MousePosition.y));


    if(MousePosition.x < 0)
        MousePosition.x = 0;
    else if(MousePosition.x > GlobalRenderer->framebuffer->Width - 16)
        MousePosition.x = GlobalRenderer->framebuffer->Width - 16;

    if(MousePosition.y < 0)
        MousePosition.y = 0;
    else if(MousePosition.y > GlobalRenderer->framebuffer->Height - 16)
        MousePosition.y = GlobalRenderer->framebuffer->Height - 16;


    //DrawMousePointer();

    MousePacketReady = false;

    // {
    //     LoadFromBuffer(oldMousePosition);
    //     //if (leftButton)
    //     //    GlobalRenderer->delChar(MousePosition.x, MousePosition.y, mouseColFront);
    //     //if (rightButton)
    //     //    GlobalRenderer->delChar(MousePosition.x, MousePosition.y, mouseColBack);
    //     SaveIntoBuffer(MousePosition);
    //     DrawMouseBuffer(MousePosition);
    //     oldMousePosition.x = MousePosition.x;
    //     oldMousePosition.y = MousePosition.y;
    // }

    bool cClicks[3] = {leftButton, rightButton, middleButton};

    bool tClicks[3] = {false, false, false};
    bool tHolds[3] = {false, false, false};
    for (int i = 0; i < 3; i++)
    {
        if (!cClicks[i])
        {
            if (clicks[i])
            {  
                clicks[i] = false; 
            }
        }
        else
        {
            if (clicks[i])
            {   
                if (PIT::TimeSinceBoot >= clickTimes[i] + 0.2)
                    tHolds[i] = true;  
            }
            else
            {
                clicks[i] = true;
                tClicks[i] = true;
                clickTimes[i] = PIT::TimeSinceBoot;
            }
        }
    }


    if(tClicks[0] || tClicks[1] || tClicks[1])
        HandleClick(tClicks[0], tClicks[1], tClicks[2]);

    if(tHolds[0] || tHolds[1] || tHolds[1])
        HandleHold(tHolds[0], tHolds[1], tHolds[2]);
}
