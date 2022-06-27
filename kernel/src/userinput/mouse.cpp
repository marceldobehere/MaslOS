#include "mouse.h"
#include "../WindowStuff/Window/window.h"
#include "../WindowStuff/WindowManager/windowManager.h"
#include "../kernelStuff/other_IO/pit/pit.h"
#include "../OSDATA/osdata.h"
#include "../Rendering/VirtualRenderer.h"
#include "../tasks/closeWindow/taskWindowClose.h"

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


kernelFiles::ZIPFile* mouseZIP;
kernelFiles::ImageFile* currentMouseImage;
const char* currentMouseImageName;
const char* oldMouseImageName = "";

bool dragArr[4] = {false, false, false, false}; //xl yu xr xd

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

uint32_t mouseColFront = Colors.white, mouseColBack = Colors.black;

bool clicks[3] = {false, false, false};
uint64_t clickTimes[3] = {0, 0, 0};


MPoint IMousePosition;
MPoint MousePosition;

// void SaveIntoBuffer(MPoint point, Framebuffer* framebuffer)
// {
//     unsigned int *pixPtr = (unsigned int*)framebuffer->BaseAddress;
//     unsigned int pps = framebuffer->PixelsPerScanLine;

//     unsigned long xoff = point.x;
//     unsigned long yoff = point.y;

//     unsigned long index = 0;

//     for (unsigned long y = yoff; y < yoff + 16; y++)
//         for (unsigned long x = xoff; x < xoff + 16; x++)
//         {
//             MouseTempBitmap[index] = *(unsigned int*)(pixPtr + x + (y * pps));
//             index++;
//         }
// }

// void LoadFromBuffer(MPoint point, Framebuffer* framebuffer)
// {
//     unsigned int *pixPtr = (unsigned int*)framebuffer->BaseAddress;
//     unsigned int pps = framebuffer->PixelsPerScanLine;

//     unsigned long xoff = point.x;
//     unsigned long yoff = point.y;

//     unsigned long index = 0;

//     for (unsigned long y = yoff; y < yoff + 16; y++)
//         for (unsigned long x = xoff; x < xoff + 16; x++)
//         {
//             *(unsigned int*)(pixPtr + x + (y * pps)) = MouseTempBitmap[index];
//             index++;
//         }
// }

// void DrawMouseBuffer(MPoint point, Framebuffer* framebuffer)
// {
//     unsigned int *pixPtr = (unsigned int*)framebuffer->BaseAddress;
//     unsigned int pps = framebuffer->PixelsPerScanLine;

//     unsigned long xoff = point.x;
//     unsigned long yoff = point.y;

//     unsigned long index = 0;
//     unsigned long index_x = 0;
//     unsigned long index_x2 = 0;
//     for (unsigned long y = yoff; y < yoff + 16; y++)
//     {
//         index_x = 0;
//         for (unsigned long x = xoff; x < xoff + 8; x++)
//         {
//             if((uint8_t)((uint8_t)(MouseShowBitmap[index] << index_x) >> 7) != 0)
//                 *(unsigned int*)(pixPtr + x + (y * pps)) = MouseDataMap[index_x2];
                
//             index_x++;
//             index_x2++;
//         }
//         index++;

//         index_x = 0;
//         for (unsigned long x = xoff + 8; x < xoff + 16; x++)
//         {
//             if((uint8_t)((uint8_t)(MouseShowBitmap[index] << index_x) >> 7) != 0)
//                 *(unsigned int*)(pixPtr + x + (y * pps)) = MouseDataMap[index_x2];
                
//             index_x++;
//             index_x2++;
//         }
//         index++;
//     }

// }

void DrawMousePointer()
{
    //DrawMousePointer1();
    //DrawMousePointer2();
}

void FigureOutCorrectMouseImage()
{
    int maxDis = 8;
    for (int i = 0; i < 4; i++)
        dragArr[i] = false;
    Window* window = WindowManager::getWindowAtMousePosition(maxDis);
    if (window == NULL)
    {
        currentMouseImageName = "default.mbif";
        
        return;
    }
    
    int minY = 22;
    if (!window->showTitleBar)
        minY = 1;

    if (IMousePosition.x >= window->position.x - maxDis && IMousePosition.x <= window->position.x + maxDis)
    {
        dragArr[0] = true;
        if (IMousePosition.y >= (window->position.y - minY) - maxDis && IMousePosition.y <= (window->position.y - minY) + maxDis)
        {
            currentMouseImageName = "drag_D_d.mbif";
            dragArr[1] = true;

        }
        else if (IMousePosition.y >= (window->position.y + window->size.height) - maxDis && IMousePosition.y <= (window->position.y + window->size.height) + maxDis)
        {
            currentMouseImageName = "drag_U_d.mbif";
            dragArr[3] = true;
        }
        else
        {
            currentMouseImageName = "drag_x.mbif";
        }
    }
    else if (IMousePosition.x >= (window->position.x + window->size.width) - maxDis && IMousePosition.x <= (window->position.x + window->size.width) + maxDis)
    {
        dragArr[2] = true;
        if (IMousePosition.y >= (window->position.y - minY) - maxDis && IMousePosition.y <= (window->position.y - minY) + maxDis)
        {
            currentMouseImageName = "drag_U_d.mbif";
            dragArr[1] = true;
        }
        else if (IMousePosition.y >= (window->position.y + window->size.height) - maxDis && IMousePosition.y <= (window->position.y + window->size.height) + maxDis)
        {
            currentMouseImageName = "drag_D_d.mbif";
            dragArr[3] = true;
        }
        else
        {
            currentMouseImageName = "drag_x.mbif";
        }
    }
    else if (IMousePosition.y >= (window->position.y - minY) - maxDis && IMousePosition.y <= (window->position.y - minY) + maxDis)
    {
        currentMouseImageName = "drag_y.mbif";
        dragArr[1] = true;
    }
    else if (IMousePosition.y >= (window->position.y + window->size.height) - maxDis && IMousePosition.y <= (window->position.y + window->size.height) + maxDis)
    {
        currentMouseImageName = "drag_y.mbif";
        dragArr[3] = true;
    }
    else
    {
        currentMouseImageName = "default.mbif";
    }
}

void DrawMousePointerNew(MPoint point, PointerFramebuffer* framebuffer)
{
    if (&osData.windows != NULL)
    {
        FigureOutCorrectMouseImage();
    }
    if (mouseZIP != NULL)
    {
        if (!StrEquals(oldMouseImageName, currentMouseImageName))
        {
            oldMouseImageName = currentMouseImageName;
            kernelFiles::ImageFile* oldMouseImage = currentMouseImage;
            currentMouseImage = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(mouseZIP, currentMouseImageName));
            
            if (oldMouseImage != NULL)
            {
                free(oldMouseImage->imageBuffer);
                free(oldMouseImage);
            }
        }
    }

    if (currentMouseImage != NULL)
        VirtualRenderer::DrawImage(currentMouseImage, point.x, point.y, 1, 1, VirtualRenderer::Border(framebuffer), framebuffer);
    else
        ;//DrawMouseBuffer(IMousePosition, framebuffer);
}

void DrawMousePointer1(PointerFramebuffer* framebuffer)
{
    //LoadFromBuffer(oldIMousePosition, framebuffer);
}

void DrawMousePointer2(PointerFramebuffer* framebuffer, MPoint mousePos)
{
    //SaveIntoBuffer(IMousePosition, framebuffer);
    //DrawMouseBuffer(IMousePosition, framebuffer);
    DrawMousePointerNew(mousePos, framebuffer);
    MousePosition = IMousePosition;
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

void InitPS2Mouse(kernelFiles::ZIPFile* _mouseZIP, const char* _mouseName)
{
    mouseZIP = _mouseZIP;
    oldMouseImageName = "";
    currentMouseImageName = _mouseName;
    mouseCycleSkip = 2;
    for (int i = 0; i < 4; i++)
        dragArr[i] = false;
    outb(0x64, 0xA8);
    Mousewait();

    for (int i = 0; i < 3; i++)
    {
        clicks[i] = false;
        clickTimes[i] = PIT::TimeSinceBootMS();
    }

    diff = MPoint();
    startDrag = false;
    dragWindow = NULL;

    IMousePosition.x = 50;
    IMousePosition.y = 50;
    MousePosition.x = 50;
    MousePosition.y = 50;
    currentMouseImage = NULL;
    //SaveIntoBuffer(IMousePosition, GlobalRenderer->framebuffer);
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
        default:
        {
            MouseCycle = 0;
            break;

        }
    }
}

bool activeDrag[4] = {false, false, false, false};
bool activeDragOn = false;

void HandleClick(bool L, bool R, bool M)
{
    AddToStack();
    //activeWindow->renderer->Println("Click");
    if (L)
    {
        Window* oldActive = activeWindow;
        Window* window = WindowManager::getWindowAtMousePosition();

        WindowActionEnum action = WindowActionEnum::_NONE;
        if (WindowManager::currentActionWindow != NULL)
        {
            if (WindowManager::currentActionWindow == window)
            {
                action = WindowManager::currentActionWindow->GetCurrentAction();
                if (action == WindowActionEnum::_NONE)
                    WindowManager::currentActionWindow = NULL;
            }
        }

        if (action != WindowActionEnum::_NONE) // Window Button Clicked
        {
            if (action == WindowActionEnum::CLOSE)
            {
                if (osData.debugTerminalWindow != WindowManager::currentActionWindow)
                {
                    // osData.debugTerminalWindow->Log("Count: {}", to_string(osData.osTasks.getCount()), Colors.yellow);
                    // osData.debugTerminalWindow->Log("Cap: {}", to_string(osData.osTasks.getCapacity()), Colors.yellow);
                    osData.osTasks.add(NewWindowCloseTask(WindowManager::currentActionWindow));
                }
                else
                {
                    osData.showDebugterminal = false;
                    osData.windowPointerThing->UpdatePointerRect(
                        osData.debugTerminalWindow->position.x - 1, 
                        osData.debugTerminalWindow->position.y - 23, 
                        osData.debugTerminalWindow->position.x + osData.debugTerminalWindow->size.width, 
                        osData.debugTerminalWindow->position.y + osData.debugTerminalWindow->size.height
                        );
                }
            }
            else if (action == WindowActionEnum::HIDE)
            {
                WindowManager::currentActionWindow->hidden = true;
                if (activeWindow == WindowManager::currentActionWindow)
                    activeWindow = NULL;
            }
        }
        else
        {
            
            Window* oldActive = activeWindow;
            activeWindow = window;
            dragWindow = window;
            startDrag = false;
            if (window != NULL)
            {
                activeDragOn = false;
                for (int i = 0; i < 4; i++)
                {
                    activeDrag[i] = dragArr[i];
                    activeDragOn |= activeDrag[i];
                }

                diff.x = MousePosition.x;
                diff.y = MousePosition.y;
                window->moveToFront = true;
                //osData.windowPointerThing->UpdateWindowBorder(osData.windows[osData.windows.getCount() - 1]);
            }
            else
            {

                //osData.windowPointerThing->UpdateWindowBorder(osData.windows[osData.windows.getCount() - 1]);


                if (Taskbar::activeTabWindow != NULL) // Taskbar Button Clicked
                {
                    activeWindow = Taskbar::activeTabWindow;
                    Taskbar::activeTabWindow->moveToFront = true;
                    activeWindow->hidden = false;
                }



            }
        }
        
        if (oldActive != NULL)
        {
            osData.windowPointerThing->UpdateWindowBorder(oldActive);
        }
    }  
    RemoveFromStack(); 
}


void HandleHold(bool L, bool R, bool M)
{
    if (L)
    {
        Window* window = dragWindow;
        if (window != NULL)
        {
            if (!startDrag && window->moveable)
            {
                startDrag = true;
            }
            else if (startDrag)
            {
                if (!activeDragOn)
                {
                    window->newPosition.x += (MousePosition.x - diff.x);
                    window->newPosition.y += (MousePosition.y - diff.y);
                }
                else
                {
                    if (activeDrag[0])
                    {
                        window->newSize.width -= (MousePosition.x - diff.x);
                        if (window->newSize.width >= 80)
                            window->newPosition.x += (MousePosition.x - diff.x);
                    }
                    if (activeDrag[1])
                    {
                        window->newSize.height -= (MousePosition.y - diff.y);
                        if (window->newSize.height >= 20)
                            window->newPosition.y += (MousePosition.y - diff.y);
                    }
                    if (activeDrag[2])
                    {
                        window->newSize.width += (MousePosition.x - diff.x);
                    }
                    if (activeDrag[3])
                    {
                        window->newSize.height += (MousePosition.y - diff.y);
                    }
                }



                diff.x = MousePosition.x;
                diff.y = MousePosition.y;
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
        IMousePosition.x += MousePacket[1];
        if (xOverflow)
            IMousePosition.x += 255;
    }
    else
    {
        MousePacket[1] = 256 - MousePacket[1];
        IMousePosition.x -= MousePacket[1];
        if (xOverflow)
            IMousePosition.x -= 255;
    }

    if (yNegative)
    {
        MousePacket[2] = 256 - MousePacket[2];
        IMousePosition.y += MousePacket[2];
        if (yOverflow)
            IMousePosition.y += 255;
    }
    else
    {
        //MousePacket[2] = 256 - MousePacket[2];
        IMousePosition.y -= MousePacket[2];
        if (yOverflow)
            IMousePosition.y -= 255;
    }

    //GlobalRenderer->overwrite = true;

    // GlobalRenderer->CursorPosition.x = 0;
    // GlobalRenderer->CursorPosition.y = 32;
    // GlobalRenderer->Println("X: {}        ", to_string((int64_t)IMousePosition.x));
    // GlobalRenderer->Println("Y: {}        ", to_string((int64_t)IMousePosition.y));


    if(IMousePosition.x < 0)
        IMousePosition.x = 0;
    else if(IMousePosition.x > GlobalRenderer->framebuffer->Width - 16)
        IMousePosition.x = GlobalRenderer->framebuffer->Width - 16;

    if(IMousePosition.y < 0)
        IMousePosition.y = 0;
    else if(IMousePosition.y > GlobalRenderer->framebuffer->Height - 16)
        IMousePosition.y = GlobalRenderer->framebuffer->Height - 16;


    //DrawMousePointer();

    MousePacketReady = false;

    // {
    //     LoadFromBuffer(oldIMousePosition);
    //     //if (leftButton)
    //     //    GlobalRenderer->delChar(IMousePosition.x, IMousePosition.y, mouseColFront);
    //     //if (rightButton)
    //     //    GlobalRenderer->delChar(IMousePosition.x, IMousePosition.y, mouseColBack);
    //     SaveIntoBuffer(IMousePosition);
    //     DrawMouseBuffer(IMousePosition);
    //     oldIMousePosition.x = IMousePosition.x;
    //     oldIMousePosition.y = IMousePosition.y;
    // }

    bool cClicks[3] = {leftButton, rightButton, middleButton};

    bool tClicks[3] = {false, false, false};
    bool tHolds[3] = {false, false, false};
    uint64_t time = PIT::TimeSinceBootMS();
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
                if (time >= clickTimes[i] + 100)
                    tHolds[i] = true;  
            }
            else
            {
                clicks[i] = true;
                tClicks[i] = true;
                clickTimes[i] = time;
            }
        }
    }


    if(tClicks[0] || tClicks[1] || tClicks[1])
        HandleClick(tClicks[0], tClicks[1], tClicks[2]);

    if(tHolds[0] || tHolds[1] || tHolds[1])
        HandleHold(tHolds[0], tHolds[1], tHolds[2]);
}
