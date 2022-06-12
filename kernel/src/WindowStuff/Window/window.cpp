#include "window.h"
#include "../../memory/heap.h"
#include "../../cmdParsing/cstrTools.h"

Window* activeWindow = NULL;

Window::Window(DefaultInstance* instance, Size size, Position position, BasicRenderer* parentRenderer, const char* title)
{
    this->instance = instance;
    this->position = position;
    this->size = size;
    this->parentRenderer = parentRenderer;
    this->parentFrameBuffer = parentRenderer->framebuffer;
    this->borderColor = Colors.gray;
    allowKeyboardDrawing = true;
    this->title = title;

    {
        framebuffer = (Framebuffer*)malloc(sizeof(Framebuffer));
        *framebuffer = Framebuffer();
        framebuffer->Height = size.height;
        framebuffer->Width = size.width;
        framebuffer->PixelsPerScanLine = size.width;
        framebuffer->BufferSize = size.height * size.width * 4;
        framebuffer->BaseAddress = malloc(framebuffer->BufferSize);
    }

    {
        renderer = (BasicRenderer*)malloc(sizeof(Framebuffer));
        *renderer = BasicRenderer(framebuffer, GlobalRenderer->psf1_font);
    }
}


void Window::Free()
{
    free(framebuffer->BaseAddress);
    free(framebuffer);
    free(renderer);
}

void Window::Render()
{
    for (int64_t y = 0; y < framebuffer->Height; y++)
    {
        for (int64_t x = 0; x < framebuffer->Width; x++)
        {
            int64_t newX = x + position.x;
            int64_t newY = y + position.y;
            if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height)
                *(uint32_t*)(parentFrameBuffer->BaseAddress + ((newX + (newY * parentFrameBuffer->Width)) * 4)) = *(uint32_t*)(framebuffer->BaseAddress + ((x + (y * framebuffer->Width)) * 4));
        }
    }


    {
        int64_t x = position.x;
        int64_t y = position.y- 21;
        parentRenderer->Clear(x,y,position.x + size.width-1,position.y-2, Colors.dgray);

        const char* stitle = StrSubstr(title, 0, size.width / 10);
        parentRenderer->putStr(stitle, x, y, Colors.white);
    }
    

    uint32_t cBorder = borderColor;
    if (activeWindow == this)
        cBorder = Colors.bgreen;

    uint8_t counter = 0;
    for (int64_t x = -1; x < framebuffer->Width + 1; x++)
    {
        int64_t newX = x + position.x;
        int64_t newY = -1 + position.y;
        if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height && (counter % 2) == 1)
            *(uint32_t*)(parentFrameBuffer->BaseAddress + ((newX + (newY * parentFrameBuffer->Width)) * 4)) = cBorder;
        
        
        newY = framebuffer->Height + position.y;
        if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height && (counter % 2) == 0)
            *(uint32_t*)(parentFrameBuffer->BaseAddress + ((newX + (newY * parentFrameBuffer->Width)) * 4)) = cBorder;

        newY = -22 + position.y;
        if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height && (counter % 2) == 0)
            *(uint32_t*)(parentFrameBuffer->BaseAddress + ((newX + (newY * parentFrameBuffer->Width)) * 4)) = cBorder;
    
        counter++;
    }

    counter = 0;
    for (int64_t y = -22; y < framebuffer->Height; y++)
    {
        int64_t newX = framebuffer->Width + position.x;
        int64_t newY = y + position.y;
        if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height && (counter % 2) == 1)
            *(uint32_t*)(parentFrameBuffer->BaseAddress + ((newX + (newY * parentFrameBuffer->Width)) * 4)) = cBorder;
        
        newX = -1 + position.x;
        if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height && (counter % 2) == 0)
            *(uint32_t*)(parentFrameBuffer->BaseAddress + ((newX + (newY * parentFrameBuffer->Width)) * 4)) = cBorder;
        
        counter++;
    } 
    
}