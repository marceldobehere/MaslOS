#include "windowManager.h"
#include "../../OSDATA/osdata.h" 
#include "../../OSDATA/osStats.h" 
#include "../../Rendering/VirtualRenderer.h"
//#include "../../userinput/mouse.h"

namespace WindowManager
{

    kernelFiles::ImageFile* windowButtonIcons[countOfButtonIcons];

    kernelFiles::ImageFile* internalWindowIcons[countOfWindowIcons];


    Window* currentActionWindow;

    Window* getWindowAtMousePosition(int dis)
    {
        AddToStack();

        if (MousePosition.y >= osData.windowPointerThing->virtualScreenBuffer->Height - osData.windowPointerThing->taskbar->Height)
        {
            RemoveFromStack();
            return NULL;
        }

        //GlobalRenderer->Println("Mouse POS Check");
        for (int64_t i = osData.windows.getCount() - 1; i >= 0; i--)
        {
            if (osData.windows[i]->hidden)
                continue;
            //GlobalRenderer->Println("Check: {}", to_string(i), Colors.yellow);
            Position tl = osData.windows[i]->position;
            Position br = Position(tl.x + osData.windows[i]->size.width, tl.y + osData.windows[i]->size.height);
            
            tl.x--;
            tl.y--;
            if (osData.windows[i]->showTitleBar)
                tl.y -= 21;
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
        this->fps = 1;
        this->actualScreenBuffer = actualScreenBuffer;
        this->background = background;

        this->defaultColor = Colors.red;
        {
            copyOfScreenBuffer = (Framebuffer*)_Malloc(sizeof(Framebuffer), "New Copy Framebuffer (struct)");
            copyOfScreenBuffer->Width = actualScreenBuffer->Width;
            copyOfScreenBuffer->Height = actualScreenBuffer->Height;
            // copyOfScreenBuffer->PixelsPerScanLine = actualScreenBuffer->PixelsPerScanLine;
            // copyOfScreenBuffer->BufferSize = actualScreenBuffer->BufferSize;
            copyOfScreenBuffer->PixelsPerScanLine = actualScreenBuffer->Width;
            copyOfScreenBuffer->BufferSize = copyOfScreenBuffer->Width * copyOfScreenBuffer->Height * 4;
            copyOfScreenBuffer->BaseAddress = _Malloc(copyOfScreenBuffer->BufferSize, "New Copy Framebuffer");
        }

        {
            virtualScreenBuffer = (PointerFramebuffer*)_Malloc(sizeof(PointerFramebuffer), "New Virtual Screenbuffer (struct)");
            virtualScreenBuffer->Width = actualScreenBuffer->Width;
            virtualScreenBuffer->Height = actualScreenBuffer->Height;
            virtualScreenBuffer->BufferSize = virtualScreenBuffer->Width * virtualScreenBuffer->Height * sizeof(uint32_t*);
            virtualScreenBuffer->BaseAddress = _Malloc(virtualScreenBuffer->BufferSize, "New Virtual Screenbuffer");
        }

        {
            copyOfVirtualBuffer = (PointerFramebuffer*)_Malloc(sizeof(PointerFramebuffer), "New Copy Virtual Screenbuffer (struct)");
            copyOfVirtualBuffer->Width = virtualScreenBuffer->Width;
            copyOfVirtualBuffer->Height = virtualScreenBuffer->Height;
            copyOfVirtualBuffer->BufferSize = virtualScreenBuffer->BufferSize;
            copyOfVirtualBuffer->BaseAddress = _Malloc(copyOfVirtualBuffer->BufferSize, "New Copy Virtual Screenbuffer");
        }

        {
            taskbar = (Framebuffer*)_Malloc(sizeof(Framebuffer), "New Taskbar Framebuffer (struct)");
            taskbar->Width = actualScreenBuffer->Width;
            taskbar->Height = 40;
            taskbar->PixelsPerScanLine = taskbar->Width;
            taskbar->BufferSize = taskbar->Height * taskbar->Width * 4;
            taskbar->BaseAddress = _Malloc(taskbar->BufferSize, "New Taskbar Framebuffer");
        }

        currentActionWindow = NULL;

        Clear(false);

    }

    void ClearFrameBuffer(Framebuffer* buffer, uint32_t col)
    {
        AddToStack();
        for (uint32_t y = 0; y < buffer->Height; y++)
            for (uint32_t x = 0; x < buffer->Width; x++)
                *((uint32_t*)buffer->BaseAddress + x + (y * buffer->PixelsPerScanLine)) = col;
        RemoveFromStack();
    }

    void ClearPointerBuffer(PointerFramebuffer* buffer, uint32_t* col)
    {
        uint32_t** endAddr = (uint32_t**)((uint64_t)buffer->BaseAddress + buffer->BufferSize);
        for (uint32_t** pixel = (uint32_t**)buffer->BaseAddress; pixel < endAddr; pixel++)
            *pixel = col;
    }

    void WindowPointerBufferThing::Clear(bool resetGlobal)
    {
        if (resetGlobal)
            ClearFrameBuffer(actualScreenBuffer, defaultBackgroundColor);
        ClearFrameBuffer(copyOfScreenBuffer, defaultBackgroundColor);
        ClearFrameBuffer(taskbar, Colors.dgray);

        ClearPointerBuffer(virtualScreenBuffer, &defaultBackgroundColor);
        ClearPointerBuffer(copyOfVirtualBuffer, &defaultBackgroundColor);
    }   

    // uint32_t* WindowPointerBufferThing::GetPixelAt(int x, int y)
    // {
    //     for (int i = osData.windows.getCount() - 1; i >= 0; i--)
    //     {
    //         Window* window = osData.windows[i];

    //         if (x >= window->position.x && y >= window->position.y && x < window->position.x + window->size.width && y < window->position.y + window->size.height)
    //         {
    //             int subX = x - window->position.x;
    //             int subY = y - window->position.y;

    //             return &((uint32_t*)window->framebuffer->BaseAddress)[subX + subY * window->framebuffer->Width];
    //         }
    //     }



    //     return &defaultBackgroundColor;
    // }

    void WindowPointerBufferThing::DrawBGRect(int x1, int y1, int x2, int y2)
    {
        if (osData.drawBackground)
        {
            for (int y = y1; y <= y2; y++)
            {
                int64_t yIndex = (((y * background->Height)/virtualScreenBuffer->Height)*background->Width);
                for (int x = x1; x <= x2; x++)
                {
                    int64_t index = x + y * virtualScreenBuffer->Width;
                    int64_t index2 = ((x * background->Width)/virtualScreenBuffer->Width) + yIndex;
                    (((uint32_t**)virtualScreenBuffer->BaseAddress)[index]) = &((uint32_t*)background->BaseAddress)[index2];//&defaultBackgroundColor;
                }
            }
        }
        else
        {
            // uint32_t** tempBuffEnd = (uint32_t**)((uint64_t)virtualScreenBuffer->BaseAddress + virtualScreenBuffer->BufferSize);
            // uint32_t* colPointer = &defaultBackgroundColor;
            // for (uint32_t** tempBuff = (uint32_t**)virtualScreenBuffer->BaseAddress; tempBuff < tempBuffEnd; tempBuff++)
            //     *tempBuff = colPointer;

            uint32_t* colPointer = &defaultBackgroundColor;
            for (int y = y1; y <= y2; y++)
            {
                int64_t yIndex = y * virtualScreenBuffer->Width;
                for (int x = x1; x <= x2; x++)
                {    
                        //int64_t index = x + y * virtualScreenBuffer->Width;
                        (((uint32_t**)virtualScreenBuffer->BaseAddress)[x + yIndex]) = colPointer;
                }   
            } 
        }
    }

    void WindowPointerBufferThing::DrawTaskbarRect(int x1, int y1, int x2, int y2)
    {
        int64_t ypos = virtualScreenBuffer->Height - taskbar->Height;

        if (y2 < ypos)
            return;
        if (y1 < ypos)
            y1 = ypos;

        for (int y = y1; y <= y2; y++)
            for (int x = x1; x <= x2; x++)
            {
                int64_t index = x + y * virtualScreenBuffer->Width;
                int64_t index2 = x + (y-ypos) * virtualScreenBuffer->Width;
                (((uint32_t**)virtualScreenBuffer->BaseAddress)[index]) = &((uint32_t*)taskbar->BaseAddress)[index2];//&defaultBackgroundColor;
            }
    }

    void WindowPointerBufferThing::UpdatePointerRect(int x1, int y1, int x2, int y2)
    {
        AddToStack();
        if (x1 < 0)
            x1 = 0;
        if (y1 < 0)
            y1 = 0;
        if (x1 >= virtualScreenBuffer->Width)
            x1 = virtualScreenBuffer->Width - 1;
        if (y1 >= virtualScreenBuffer->Height)
            y1 = virtualScreenBuffer->Height - 1;

        if (x2 < 0)
            x2 = 0;
        if (y2 < 0)
            y2 = 0;
        if (x2 >= virtualScreenBuffer->Width)
            x2 = virtualScreenBuffer->Width - 1;
        if (y2 >= virtualScreenBuffer->Height)
            y2 = virtualScreenBuffer->Height - 1;


        if (x1 > x2 || y1 > y2)
        {
            RemoveFromStack();
            return;
        }


        DrawBGRect(x1, y1, x2, y2);
        

        int count = osData.windows.getCount();
        for (int i = 0; i < count; i++)
            RenderWindowRect(osData.windows[i], x1, y1, x2, y2);

        DrawTaskbarRect(x1, y1, x2, y2);
        RemoveFromStack();
    }


    void WindowPointerBufferThing::RenderWindowRect(Window* window, int x1, int y1, int x2, int y2)
    {
        AddToStack();

        if (window == NULL)
        {
            RemoveFromStack();
            return;
        }

        if (window->hidden)
        {
            RemoveFromStack();
            return;
        }

        if (window == osData.debugTerminalWindow && !osData.showDebugterminal)
        {
            RemoveFromStack();
            return;
        }

        int64_t _x1 = window->position.x;
        int64_t _y1 = window->position.y;
        int64_t _x2 = _x1 + window->size.width - 1;
        int64_t _y2 = _y1 + window->size.height - 1;

        if ((_x1 - 1) > x2 || (_x2 + 1) < x1 || (_y1 - 23) > y2 || (_y2 + 1) < y1)
        {
            RemoveFromStack();
            return;
        }

        { 
            if (y1 > _y1)
                _y1 = y1;

            if (y2 < _y2)
                _y2 = y2;

            if (x1 > _x1)
                _x1 = x1;

            if (x2 < _x2)
                _x2 = x2;


            if (0 > _y1)
                _y1 = 0;

            if (virtualScreenBuffer->Height <= _y2)
                _y2 = virtualScreenBuffer->Height - 1;

            if (0 > _x1)
                _x1 = 0;

            if (virtualScreenBuffer->Width <= _x2)
                _x2 = virtualScreenBuffer->Width - 1;


            for (int64_t y = _y1; y <= _y2; y++)
            {
                //GlobalRenderer->Print("Y");
                for (int64_t x = _x1; x <= _x2; x++)
                {
                    //GlobalRenderer->Print("X");
                    int64_t index = x + y * virtualScreenBuffer->Width; 
                    int64_t index2 = (x - window->position.x) + (y - window->position.y) * window->framebuffer->Width; 
                    
                    (((uint32_t**)virtualScreenBuffer->BaseAddress)[index]) = &((uint32_t*)window->framebuffer->BaseAddress)[index2];
                }
            }

        }


        {
            _x1 = window->position.x - 1;
            _y1 = window->position.y - 23;
            _x2 = _x1 + window->size.width + 2;//3;
            _y2 = _y1 + window->size.height + 24;//25;
        
            if (y1 > _y1)
                _y1 = y1;

            if (y2 < _y2)
                _y2 = y2;

            if (x1 > _x1)
                _x1 = x1;

            if (x2 < _x2)
                _x2 = x2;

            if (0 > _y1)
                _y1 = 0;

            if (virtualScreenBuffer->Height <= _y2)
                _y2 = virtualScreenBuffer->Height - 1;

            if (0 > _x1)
                _x1 = 0;

            if (virtualScreenBuffer->Width <= _x2)
                _x2 = virtualScreenBuffer->Width - 1;

        }



        if (_x1 > _x2 || _y1 > _y2)
        {
            RemoveFromStack();
            return;
        }

        if (window->showTitleBar)
        {
            int64_t x = window->position.x;
            int64_t y = window->position.y- 21;
            VirtualRenderer::Clear(x,y, x + window->size.width-1, window->position.y-2, VirtualRenderer::Border(_x1, _y1, _x2, _y2), virtualScreenBuffer, &window->defaultTitleBackgroundColor);

            const char* stitle = StrSubstr(window->title, 0, (window->size.width - 60) / 8);


            // if (window->instance != NULL)
            // {
            //     if (window->instance->instanceType == InstanceType::Terminal)
            //     {
            //         TerminalInstance* terminal = (TerminalInstance*)window->instance;
            //         free((void*)stitle);
            //         stitle = StrCopy(to_string(terminal->tasks.getCount()));
            //     }
            // }

            if (activeWindow == window)
                VirtualRenderer::putStr(stitle, x, y, VirtualRenderer::Border(_x1, _y1, _x2, _y2), virtualScreenBuffer, &window->selectedTitleColor);
            else
                VirtualRenderer::putStr(stitle, x, y, VirtualRenderer::Border(_x1, _y1, _x2, _y2), virtualScreenBuffer, &window->defaultTitleColor);
            _Free((void*)stitle);
        }


        
        if (window->showBorder)
        {
            uint32_t** arr = ((uint32_t**)virtualScreenBuffer->BaseAddress);
            int64_t width = virtualScreenBuffer->Width;

            uint32_t* cBorder = &window->defaultBorderColor;
            if (activeWindow == window)
                cBorder = &window->selectedBorderColor;

            uint8_t counter = 0;
            for (int64_t x = -1; x < window->size.width + 1; x++)
            {
                int64_t newX = x + window->position.x;
                int64_t newY = -1 + window->position.y;
                if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 1)
                    arr[newX + newY * width] = cBorder; //*(uint32_t*)(to->BaseAddress + ((newX + (newY * to->Width)) * 4))
                
                
                newY = window->size.height + window->position.y;
                if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 0)
                    arr[newX + newY * width] = cBorder;
                
                // if (window->showTitleBar)
                // {
                //     newY = -22 + window->position.y;
                //     if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 0)
                //         arr[newX + newY * width] = cBorder;
                // }
            
                counter++;
            }

            if (window->showTitleBar)
            {
                counter = 0;
                for (int64_t x = -1; x < window->size.width + 1; x++)
                {
                    int64_t newX = x + window->position.x;
                    int64_t newY = -22 + window->position.y;
                        
                    if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 0)
                        arr[newX + newY * width] = cBorder;
                    
                    counter++;
                }  
            }


            counter = 0;
            int64_t maxY = -22;
            if (!window->showTitleBar)
                maxY = -1;
            for (int64_t y = maxY; y < window->size.height; y++)
            {
                int64_t newX = window->size.width + window->position.x;
                int64_t newY = y + window->position.y;
                if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 1)
                    arr[newX + newY * width] = cBorder;
                
                newX = -1 + window->position.x;
                if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 0)
                    arr[newX + newY * width] = cBorder;
                
                counter++;
            } 
        }

        
        if (window->showTitleBar)
        {
            VirtualRenderer::Border border = VirtualRenderer::Border(_x1, _y1, _x2, _y2);
            int64_t x = window->position.x + window->size.width;
            int64_t y = window->position.y - 22;

            
            {
                int state = 0;
                if (activeWindow == window)
                    state = 1;
                if (MousePosition.x >= x - 20 && MousePosition.x <= x && MousePosition.y >= y && MousePosition.y <= y + 20)
                    state = 2;
                if (state == 2)
                    currentActionWindow = window;

                VirtualRenderer::DrawImage(windowButtonIcons[windowButtonIconEnum.CLOSE_N + state], x - 20, y + 1, 1, 1, border, virtualScreenBuffer);
                x -= 20;
            }
            {
                int state = 0;
                if (activeWindow == window)
                    state = 1;
                if (MousePosition.x >= x - 20 && MousePosition.x <= x && MousePosition.y >= y && MousePosition.y <= y + 20)
                    state = 2;
                if (state == 2)
                    currentActionWindow = window;

                VirtualRenderer::DrawImage(windowButtonIcons[windowButtonIconEnum.MIN_N + state], x - 20, y + 1, 1, 1, border, virtualScreenBuffer);
                x -= 20;
            }
            {
                int state = 0;
                if (activeWindow == window)
                    state = 1;
                if (MousePosition.x >= x - 20 && MousePosition.x <= x && MousePosition.y >= y && MousePosition.y <= y + 20)
                    state = 2;
                if (state == 2)
                    currentActionWindow = window;

                VirtualRenderer::DrawImage(windowButtonIcons[windowButtonIconEnum.HIDE_N + state], x - 20, y + 1, 1, 1, border, virtualScreenBuffer);
                x -= 20;
            }

            
        }

        {
            //uint32_t col = Colors.bred;
            //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 5, _x1 + 20, _y1 + 10, border, virtualScreenBuffer,  &window->defaultBorderColor);
            //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 10, _x1 + 1, _y1 + 11, border, virtualScreenBuffer, &window->selectedBorderColor);
            //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 14, _x1 + 8, _y1 + 17, border, virtualScreenBuffer, &window->selectedBorderColor);
            //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 18, _x1 + 6, _y1 + 25, border, virtualScreenBuffer, &window->defaultBorderColor);
            

            //windowIcons[windowIconEnum.CLOSE_N]

            //VirtualRenderer::DrawImage(kernelFiles::ConvertFileToImage(&osData.windowIconsZIP->files[0]), x - 30, y + 10, 1, 1, border, virtualScreenBuffer);
            //VirtualRenderer::DrawImage(kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(osData.windowIconsZIP, WindowManager::windowIconNames[3])), x - 60, y + 40, 1, 1, border, virtualScreenBuffer);
            //osData.debugTerminalWindow->Log("- ADDR A: {}", ConvertHexToString((uint64_t)windowIcons[0]), Colors.yellow);
            //VirtualRenderer::DrawImage(windowIcons[windowIconEnum.MIN_H], x - 90, y + 50, 2, 2, border, virtualScreenBuffer);

            //VirtualRenderer::Border border = VirtualRenderer::Border(_x1, _y1, _x2, _y2);
            //int64_t x = window->position.x + window->size.width;
            //uint32_t col = Colors.bred;
            //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 5, _x1 + 20, _y1 + 10, border, virtualScreenBuffer,  &window->defaultBorderColor);
            //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 10, _x1 + 1, _y1 + 11, border, virtualScreenBuffer, &window->selectedBorderColor);
            //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 14, _x1 + 8, _y1 + 17, border, virtualScreenBuffer, &window->selectedBorderColor);
            //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 18, _x1 + 6, _y1 + 25, border, virtualScreenBuffer, &window->defaultBorderColor);
            

        }








        //GlobalRenderer->Println("\nDone.");
        RemoveFromStack();
    } 

/*
if (window != NULL)
{
    if (false)
    {
        int64_t x = pos.x;
        int64_t y = pos.y- 21;
        window->parentRenderer->Clear(x,y,pos.x + size.width-1, pos.y-2, Colors.dgray);

        const char* stitle = title;//StrSubstr(title, 0, size.width / 10);


        // if (window->instance != NULL)
        // {
        //     if (window->instance->instanceType == InstanceType::Terminal)
        //     {
        //         TerminalInstance* terminal = (TerminalInstance*)window->instance;
        //         free((void*)stitle);
        //         stitle = StrCopy(to_string(terminal->tasks.getCount()));
        //     }
        // }

        if (activeWindow == this)
            window->parentRenderer->putStr(stitle, x, y, Colors.white);
        else
            window->parentRenderer->putStr(stitle, x, y, Colors.bgray);
        //free((void*)stitle);
    }
    

    uint32_t cBorder = borderColor;
    if (activeWindow == window)
        cBorder = Colors.bgreen;

    uint8_t counter = 0;
    for (int64_t x = -1; x < size.width + 1; x++)
    {
        int64_t newX = x + pos.x;
        int64_t newY = -1 + pos.y;
        if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 1)
            *(uint32_t*)(to->BaseAddress + ((newX + (newY * to->Width)) * 4)) = cBorder;
        
        
        newY = size.height + pos.y;
        if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 0)
            *(uint32_t*)(to->BaseAddress + ((newX + (newY * to->Width)) * 4)) = cBorder;

        newY = -22 + pos.y;
        if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 0)
            *(uint32_t*)(to->BaseAddress + ((newX + (newY * to->Width)) * 4)) = cBorder;
    
        counter++;
    }

    counter = 0;
    for (int64_t y = -22; y < size.height; y++)
    {
        int64_t newX = size.width + pos.x;
        int64_t newY = y + pos.y;
        if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 1)
            *(uint32_t*)(to->BaseAddress + ((newX + (newY * to->Width)) * 4)) = cBorder;
        
        newX = -1 + pos.x;
        if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 0)
            *(uint32_t*)(to->BaseAddress + ((newX + (newY * to->Width)) * 4)) = cBorder;
        
        counter++;
    } 
}
*/



    void WindowPointerBufferThing::UpdateWindowRect(Window* window)
    {
        UpdatePointerRect(window->position.x - 1, window->position.y - 24, window->position.x + window->size.width + 1, window->position.y + window->size.height + 1);
    }

    void WindowPointerBufferThing::RenderWindow(Window* window)
    {
        int x1 = max(0, window->position.x - 1);
        int y1 = max(0, window->position.y - 24);
        int x2 = min(virtualScreenBuffer->Width - 1, window->position.x + window->size.width + 1);
        int y2 = min(virtualScreenBuffer->Height- 1, window->position.y + window->size.height + 1);

        RenderWindowRect(window, 
            x1, y1, 
            x2, y2
        );
        
        DrawTaskbarRect(x1, y1, x2, y2);
        //0, 0, actualScreenBuffer->Width - 1, actualScreenBuffer->Height - 1);
        // AddToStack();
        // uint32_t* pixel = (uint32_t*)window->framebuffer->BaseAddress;

        // int64_t x1 = window->position.x;
        // int64_t y1 = window->position.y;
        // int64_t x2 = x1 + window->size.width;
        // int64_t y2 = y1 + window->size.height;

        // // GlobalRenderer->Println("X1: {}", to_string(x1), Colors.white);
        // // GlobalRenderer->Println("Y1: {}", to_string(y1), Colors.white);
        // // GlobalRenderer->Println("X2: {}", to_string(x2), Colors.white);
        // // GlobalRenderer->Println("Y2: {}", to_string(y2), Colors.white);

    
        // // GlobalRenderer->Println("Frame ADDR 1: {}", ConvertHexToString((uint64_t)window->framebuffer->BaseAddress), Colors.white);
        // // GlobalRenderer->Println("Frame ADDR 2: {}", ConvertHexToString((uint64_t)virtualScreenBuffer->BaseAddress), Colors.white);

        // for (int64_t y = y1; y < y2; y++)
        // {
        //     //GlobalRenderer->Print("Y");
        //     for (int64_t x = x1; x < x2; x++)
        //     {
        //         //GlobalRenderer->Print("X");
        //         int64_t index = x + y * virtualScreenBuffer->Width; 
                
        //         // if ((((uint32_t**)copyOfVirtualBuffer->BaseAddress)[index]) != pixel)
        //         // {
        //         //     (((uint32_t**)virtualScreenBuffer->BaseAddress)[index]) = pixel;
        //         //     (((uint32_t**)copyOfVirtualBuffer->BaseAddress)[index]) = pixel;
        //         // }

        //         if (x >= 0 && y >= 0 && x < virtualScreenBuffer->Width && y < virtualScreenBuffer->Height)
        //             (((uint32_t**)virtualScreenBuffer->BaseAddress)[index]) = pixel;
        //         pixel++;
        //     }
        // }
        // //GlobalRenderer->Println("\nDone.");
        // RemoveFromStack();
    } 

    void WindowPointerBufferThing::RenderWindows()
    {
        UpdatePointerRect(0, 0, virtualScreenBuffer->Width - 1, virtualScreenBuffer->Height - 1);
        //for (int i = 0; i < osData.windows.getCount(); i++)
        //    RenderWindow(osData.windows[i]);
    } 

    uint8_t testInterlace = 4;
    uint8_t testCounterX = 0;
    uint8_t testCounterY = 0;



    // inline uint64_t WindowPointerBufferThing::RenderActualSquare(int _x1, int _y1, int _x2, int _y2)
    // {
    //     AddToStack();
    //     uint64_t counta = 0;
    //     uint64_t h = actualScreenBuffer->Height, w = actualScreenBuffer->Width, bpl = actualScreenBuffer->PixelsPerScanLine;
    //     uint64_t xdiff = _x2 - _x1;
    //     uint32_t** vPixel = (uint32_t**)virtualScreenBuffer->BaseAddress + _x1 + w * _y1;
    //     uint32_t*  cPixel = (uint32_t*)  copyOfScreenBuffer->BaseAddress + _x1 + w * _y1;

    //     // DRAW SQUARE
    //     for     (uint64_t y1 = _y1; y1 <= _y2; y1++)
    //     {
    //         for (uint64_t x1 = _x1; x1 <= _x2; x1++)
    //         {
    //             uint32_t col = **vPixel;
    //             if (*cPixel != col)
    //             {
    //                 *cPixel = col;
    //                 *(((uint32_t*) actualScreenBuffer->BaseAddress) + x1 + y1 * bpl) = col; //counta + 0xff111111;
    //                 counta++;
    //             }
    //             vPixel++;
    //             cPixel++;
    //         }
    //         vPixel += w - (xdiff+1);
    //         cPixel += w - (xdiff+1);
    //     }

    //     RemoveFromStack();
    //     return counta;
    // }

    void WindowPointerBufferThing::Render()
    {
        AddToStack();
        uint64_t counta = 0;

        // {
        //     uint32_t** vPixel = (uint32_t**)virtualScreenBuffer->BaseAddress;
        //     vPixel += (actualScreenBuffer->Height - taskbar->Height) * actualScreenBuffer->Width;
        //     uint32_t*  tPixel = (uint32_t*) taskbar->BaseAddress;

        //     int64_t count = taskbar->Height*actualScreenBuffer->Width;
        //     for (int64_t i = 0; i < count; i++)
        //     {
        //         *vPixel = tPixel;
        //         vPixel++;
        //     }
        // }
        if (testInterlace != 1 && testInterlace != 0)
        {
            AddToStack();
            uint64_t h = actualScreenBuffer->Height, w = actualScreenBuffer->Width;

            uint32_t** vPixel = (uint32_t**)virtualScreenBuffer->BaseAddress + w * testCounterY;// + testCounterX;
            uint32_t*  cPixel = (uint32_t*)  copyOfScreenBuffer->BaseAddress + w * testCounterY;// + testCounterX;

            uint8_t testInterlaceMinusOne = testInterlace - 1;
            uint64_t wTimesInterlaceMinusOne = w * testInterlaceMinusOne;

            // uint32_t** bVPixel = (uint32_t**)virtualScreenBuffer->BaseAddress;// + w * testCounterY;
            // uint32_t* bCPixel = (uint32_t*) copyOfScreenBuffer->BaseAddress;// + w * testCounterY;


            for (int64_t y = testCounterY; y < h; y += testInterlace)
            {
                for (int64_t x = testCounterX; x < w;)
                {
                    uint32_t col = *(vPixel[x]);
                    if (cPixel[x] != col)
                    {
                        // BEFORE
                        // x -= testCounterX;
                        // y -= testCounterY;
                        // uint32_t** vPixel1 = vPixel;
                        // uint32_t*  cPixel1 = cPixel;
                        // vPixel -= testCounterX + (testCounterY * w);
                        // cPixel -= testCounterX + (testCounterY * w); 

                        // counta += RenderActualSquare(
                        //                                         max(x, 0), 
                        //                                         max(y, 0), 
                        //                                         min(x + testInterlace, w - 1), 
                        //                                         min(y + testInterlace, h - 1)
                        //                                     );

                        //AddToStack();
                        counta += RenderActualSquare(
                            x - (testInterlace * 2 - 1), 
                            y - (testInterlace * 2 - 1), 
                            
                            x + testInterlace * 4 - 2, 
                            y + testInterlace * 4 - 2
                            );
                        //RemoveFromStack();

                        // vPixel += testInterlace * 2;
                        // cPixel += testInterlace * 2;
                        x       += testInterlace * 3;

                        // AFTER
                        // x += testCounterX;
                        // y += testCounterY;
                        // vPixel = vPixel1;
                        // cPixel = cPixel1;
                    }
                    // vPixel += testInterlace;
                    // cPixel += testInterlace;
                    x += testInterlace;
                } 
                // vPixel += wTimesInterlaceMinusOne;
                // cPixel += wTimesInterlaceMinusOne;
                vPixel += testInterlace * w;
                cPixel += testInterlace * w;
            }
            RemoveFromStack();
        }
        else
        {
            if (testInterlace == 1)
            {
                AddToStack();
                uint64_t h = actualScreenBuffer->Height, w = actualScreenBuffer->Width;

                uint32_t** vPixel = (uint32_t**)virtualScreenBuffer->BaseAddress;
                uint32_t*  cPixel = (uint32_t*)  copyOfScreenBuffer->BaseAddress;
                uint32_t*  aPixel = (uint32_t*)  actualScreenBuffer->BaseAddress;

                AddToStack();
                for (int64_t y = 0; y < h; y++)
                {
                    aPixel = (uint32_t*)  actualScreenBuffer->BaseAddress + y * actualScreenBuffer->PixelsPerScanLine;
                    for (int64_t x = 0; x < w; x++)
                    {
                        uint32_t col = **vPixel;
                        if (*cPixel != col)
                        {
                            *aPixel = col;
                            *cPixel = col;
                        }
                        vPixel++;
                        cPixel++;
                        aPixel++;
                    } 
                }
                RemoveFromStack();

                RemoveFromStack();
            }
            else
            {
                AddToStack();
                uint64_t h = actualScreenBuffer->Height, w = actualScreenBuffer->Width;

                uint32_t** vPixel = (uint32_t**)virtualScreenBuffer->BaseAddress;
                uint32_t*  aPixel = (uint32_t*)  actualScreenBuffer->BaseAddress;

                AddToStack();
                for (int64_t y = 0; y < h; y++)
                {
                    aPixel = (uint32_t*)  actualScreenBuffer->BaseAddress + y * actualScreenBuffer->PixelsPerScanLine;
                    for (int64_t x = 0; x < w; x++)
                    {
                        *aPixel = **vPixel;
                        vPixel++;
                        aPixel++;
                    } 
                }
                RemoveFromStack();

                RemoveFromStack();
            }
        }

        AddToStack();
        //osData.debugTerminalWindow->Log("             : ################", Colors.black);
        osData.debugTerminalWindow->renderer->CursorPosition.x = 0;
        osData.debugTerminalWindow->renderer->CursorPosition.y -= 16 * 12;

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 200,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        osData.debugTerminalWindow->Log("Pixel changed: {}", to_string(counta), Colors.yellow);
        RemoveFromStack();

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 200,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        osData.debugTerminalWindow->Log("FPS: {}", to_string(fps), Colors.yellow);
        RemoveFromStack();

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 200,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        osData.debugTerminalWindow->Log("Heap count: {}", to_string(heapCount), Colors.yellow);
        RemoveFromStack();

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 200,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        osData.debugTerminalWindow->Log("Used Heap count: {}", to_string(usedHeapCount), Colors.yellow);
        RemoveFromStack();

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        osData.debugTerminalWindow->Log("Mouse Packet Count: {}", to_string(mousePackets.getCount()), Colors.yellow);
        RemoveFromStack();

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        osData.debugTerminalWindow->Log("Malloc Count: {}", to_string(mallocCount), Colors.yellow);
        RemoveFromStack();

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        osData.debugTerminalWindow->Log("Free Count: {}", to_string(freeCount), Colors.yellow);
        RemoveFromStack();

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        osData.debugTerminalWindow->Log("Stack Trace Count: {}", to_string(MStackData::stackPointer+1), Colors.yellow);
        RemoveFromStack();

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        osData.debugTerminalWindow->Log("Last Free Size: {}", to_string(lastFreeSize), Colors.yellow);
        RemoveFromStack();

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        osData.debugTerminalWindow->Log("PORT 64 VAL: {}", ConvertHexToString(osData.port64Val), Colors.yellow);
        RemoveFromStack();

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        
        uint64_t tVal = 0;

        if (MStackData::BenchmarkMode == 0)
            tVal = MStackData::BenchmarkStackPointer2;//osData.debugTerminalWindow->Log("MSTACK: {}", to_string(MStackData::BenchmarkStackPointer2), Colors.yellow);
        else
            tVal = MStackData::BenchmarkStackPointer1;//osData.debugTerminalWindow->Log("MSTACK: {}", to_string(MStackData::BenchmarkStackPointer1), Colors.yellow);
        if (tVal > MStackData::BenchmarkStackPointerSave)
        {
            SaveBenchmarkStack(((MStackData::BenchmarkMode + 1) % 2));
        }

        osData.debugTerminalWindow->Log("MSTACK: {}", to_string(tVal), Colors.yellow);
        RemoveFromStack();

        AddToStack();
        osData.debugTerminalWindow->renderer->Clear(
            osData.debugTerminalWindow->renderer->CursorPosition.x,
            osData.debugTerminalWindow->renderer->CursorPosition.y,
            osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
            osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
            Colors.black);
        osData.debugTerminalWindow->Log("MSTACK MAX: {}", to_string(MStackData::BenchmarkStackPointerSave), Colors.yellow);
        RemoveFromStack();

        RemoveFromStack();

     
        AddToStack();
        if(++testCounterX >= testInterlace)
        {
            testCounterX = 0;
            if(++testCounterY >= testInterlace)
                testCounterY = 0;
        }   
        RemoveFromStack();

        RemoveFromStack();
    }


    void WindowPointerBufferThing::UpdateWindowBorder(Window* window)
    {
        if (!window->showBorder)
            return;

        int x1 = window->position.x;
        int y1 = window->position.y;
        int x2 = x1 + window->size.width;
        int y2 = y1 + window->size.height;
        
        osData.windowPointerThing->UpdatePointerRect(x1-1, y1-22, 
                                                     x1-1, y2);
        osData.windowPointerThing->UpdatePointerRect(x2,   y1-22, 
                                                     x2,   y2);

        osData.windowPointerThing->UpdatePointerRect(x1-1, y1-22, 
                                                     x2,   y1-22);
        osData.windowPointerThing->UpdatePointerRect(x1-1, y1-1, 
                                                     x2,   y1-1);
        osData.windowPointerThing->UpdatePointerRect(x1-1, y2, 
                                                     x2,   y2);

        osData.windowPointerThing->UpdatePointerRect(x1,   y1-22, 
                                                    x2,   y1-1);



        // osData.windowPointerThing->UpdatePointerRect(x1-1, y1-22, 
        //                                              x1-1, y2);
        // osData.windowPointerThing->UpdatePointerRect(x2,   y1-22, 
        //                                              x2,   y2);

        // osData.windowPointerThing->UpdatePointerRect(x1-1, y1-22, 
        //                                              x2,   y1-22);
        // osData.windowPointerThing->UpdatePointerRect(x1-1, y1-1, 
        //                                              x2,   y1-1);
        // osData.windowPointerThing->UpdatePointerRect(x1-1, y2, 
        //                                              x2,   y2);
    }
}

