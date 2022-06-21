#include "windowManager.h"
#include "../../OSDATA/osdata.h" 

//#include "../../userinput/mouse.h"

namespace WindowManager
{

    Window* getWindowAtMousePosition(int dis)
    {
        AddToStack("", "WindowStuff/WindowManager/windowManager.cpp");
        //GlobalRenderer->Println("Mouse POS Check");
        for (int64_t i = osData.windows.getCount() - 1; i >= 0; i--)
        {
            //GlobalRenderer->Println("Check: {}", to_string(i), Colors.yellow);
            Position tl = osData.windows[i]->position;
            Position br = Position(tl.x + osData.windows[i]->size.width, tl.y + osData.windows[i]->size.height);
            
            tl.x--;
            tl.y -= 22;
            br.x++;
            br.y++;

            tl.x -= dis;
            tl.y -= dis;
            br.x += dis;
            br.y += dis;


            if (MousePosition.x >= tl.x && MousePosition.x <= br.x && MousePosition.y >= tl.y && MousePosition.y <=br.y)
            {
                RemoveFromStack();
                return osData.windows[i];
            }
        }
        //GlobalRenderer->Println("NONE");
        RemoveFromStack();
        return NULL;   
    }

    Window* getWindowAtMousePosition()
    {
        return getWindowAtMousePosition(8);
    }



/*
    O uint32_t defaultColor = Colors.black;
    O uint32_t defaultBackgroundColor = defaultColor;
    O Framebuffer* actualScreenBuffer;
    O Framebuffer* copyOfScreenBuffer;
    O Framebuffer* background;
    O PointerFramebuffer* virtualScreenBuffer;
      PointerFramebuffer* copyOfVirtualBuffer;
*/

    WindowPointerBufferThing::WindowPointerBufferThing(Framebuffer* actualScreenBuffer, Framebuffer* background, uint32_t backgroundColor)
    {
        this->defaultBackgroundColor = backgroundColor;
        this->actualScreenBuffer = actualScreenBuffer;
        this->background = background;

        this->defaultColor = Colors.red;
        {
            copyOfScreenBuffer = (Framebuffer*)malloc(sizeof(Framebuffer));
            copyOfScreenBuffer->Width = actualScreenBuffer->Width;
            copyOfScreenBuffer->Height = actualScreenBuffer->Height;
            copyOfScreenBuffer->PixelsPerScanLine = actualScreenBuffer->PixelsPerScanLine;
            copyOfScreenBuffer->BufferSize = actualScreenBuffer->BufferSize;
            copyOfScreenBuffer->BaseAddress = malloc(copyOfScreenBuffer->BufferSize);
        }

        {
            virtualScreenBuffer = (PointerFramebuffer*)malloc(sizeof(PointerFramebuffer));
            virtualScreenBuffer->Width = actualScreenBuffer->Width;
            virtualScreenBuffer->Height = actualScreenBuffer->Height;
            virtualScreenBuffer->BufferSize = virtualScreenBuffer->Width * virtualScreenBuffer->Height * sizeof(uint32_t*);
            virtualScreenBuffer->BaseAddress = malloc(virtualScreenBuffer->BufferSize);
        }

        {
            copyOfVirtualBuffer = (PointerFramebuffer*)malloc(sizeof(PointerFramebuffer));
            copyOfVirtualBuffer->Width = virtualScreenBuffer->Width;
            copyOfVirtualBuffer->Height = virtualScreenBuffer->Height;
            copyOfVirtualBuffer->BufferSize = virtualScreenBuffer->BufferSize;
            copyOfVirtualBuffer->BaseAddress = malloc(copyOfVirtualBuffer->BufferSize);
        }

        Clear();

    }

    void ClearFrameBuffer(Framebuffer* buffer, uint32_t col)
    {
        uint32_t* endAddr = (uint32_t*)((uint64_t)buffer->BaseAddress + buffer->BufferSize);
        for (uint32_t* pixel = (uint32_t*)buffer->BaseAddress; pixel < endAddr; pixel++)
            *pixel = col;
    }

    void ClearPointerBuffer(PointerFramebuffer* buffer, uint32_t* col)
    {
        uint32_t** endAddr = (uint32_t**)((uint64_t)buffer->BaseAddress + buffer->BufferSize);
        for (uint32_t** pixel = (uint32_t**)buffer->BaseAddress; pixel < endAddr; pixel++)
            *pixel = col;
    }

    void WindowPointerBufferThing::Clear()
    {
        ClearFrameBuffer(actualScreenBuffer, defaultBackgroundColor);
        ClearFrameBuffer(copyOfScreenBuffer, defaultBackgroundColor);

        ClearPointerBuffer(virtualScreenBuffer, &defaultBackgroundColor);
        ClearPointerBuffer(copyOfVirtualBuffer, &defaultBackgroundColor);
    }   

    uint32_t* WindowPointerBufferThing::GetPixelAt(int x, int y)
    {
        for (int i = osData.windows.getCount() - 1; i >= 0; i--)
        {
            Window* window = osData.windows[i];

            if (x >= window->position.x && y >= window->position.y && x < window->position.x + window->size.width && y < window->position.y + window->size.height)
            {
                int subX = x - window->position.x;
                int subY = y - window->position.y;

                return &((uint32_t*)window->framebuffer->BaseAddress)[subX + subY * window->framebuffer->Width];
            }
        }



        return &defaultBackgroundColor;
    }

    void WindowPointerBufferThing::UpdatePointerRect(int x1, int y1, int x2, int y2)
    {
        for (int64_t y = y1; y <= y2; y++)
        {
            for (int64_t x = x1; x <= x2; x++)
            {
                int64_t index = x + y * virtualScreenBuffer->Width; 
                if (x >= 0 && y >= 0 && x < virtualScreenBuffer->Width && y < virtualScreenBuffer->Height)
                    (((uint32_t**)virtualScreenBuffer->BaseAddress)[index]) = GetPixelAt(x, y);
            }
        }
    }

    void WindowPointerBufferThing::RenderWindow(Window* window)
    {
        AddToStack("RenderWindwow", "WindowStuff/WindowManager/windowManager.cpp");
        uint32_t* pixel = (uint32_t*)window->framebuffer->BaseAddress;

        int64_t x1 = window->position.x;
        int64_t y1 = window->position.y;
        int64_t x2 = x1 + window->size.width;
        int64_t y2 = y1 + window->size.height;

        // GlobalRenderer->Println("X1: {}", to_string(x1), Colors.white);
        // GlobalRenderer->Println("Y1: {}", to_string(y1), Colors.white);
        // GlobalRenderer->Println("X2: {}", to_string(x2), Colors.white);
        // GlobalRenderer->Println("Y2: {}", to_string(y2), Colors.white);

    
        // GlobalRenderer->Println("Frame ADDR 1: {}", ConvertHexToString((uint64_t)window->framebuffer->BaseAddress), Colors.white);
        // GlobalRenderer->Println("Frame ADDR 2: {}", ConvertHexToString((uint64_t)virtualScreenBuffer->BaseAddress), Colors.white);

        for (int64_t y = y1; y < y2; y++)
        {
            //GlobalRenderer->Print("Y");
            for (int64_t x = x1; x < x2; x++)
            {
                //GlobalRenderer->Print("X");
                int64_t index = x + y * virtualScreenBuffer->Width; 
                
                // if ((((uint32_t**)copyOfVirtualBuffer->BaseAddress)[index]) != pixel)
                // {
                //     (((uint32_t**)virtualScreenBuffer->BaseAddress)[index]) = pixel;
                //     (((uint32_t**)copyOfVirtualBuffer->BaseAddress)[index]) = pixel;
                // }

                if (x >= 0 && y >= 0 && x < virtualScreenBuffer->Width && y < virtualScreenBuffer->Height)
                    (((uint32_t**)virtualScreenBuffer->BaseAddress)[index]) = pixel;
                pixel++;
            }
        }
        //GlobalRenderer->Println("\nDone.");
        RemoveFromStack();
    } 

    void WindowPointerBufferThing::RenderWindows()
    {
        for (int i = 0; i < osData.windows.getCount(); i++)
            RenderWindow(osData.windows[i]);
    } 

    void WindowPointerBufferThing::Render()
    {
        uint32_t** vPixel = (uint32_t**)virtualScreenBuffer->BaseAddress;
        uint32_t*  aPixel = (uint32_t*) actualScreenBuffer->BaseAddress;
        uint32_t*  cPixel = (uint32_t*) copyOfScreenBuffer->BaseAddress;

        int64_t count = actualScreenBuffer->Height*actualScreenBuffer->Width;
        for (int64_t i = 0; i < count; i++)
        {
            uint32_t col = **vPixel;
            if (*cPixel != col)
            {
                *cPixel = col;
                *aPixel = col;
            }
            vPixel++;
            aPixel++;
            cPixel++;
        }
    }

}

