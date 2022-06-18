#include "window.h"
#include "../../memory/heap.h"
#include "../../cmdParsing/cstrTools.h"
#include "../../OSDATA/osdata.h"

Window* activeWindow = NULL;

Window::Window(DefaultInstance* instance, Size size, Position position, BasicRenderer* parentRenderer, const char* title)
{
    AddToStack("Constructor", "WindowStuff/Window/window.cpp");
    this->instance = instance;
    this->position = position;
    this->size = size;
    this->newPosition = position;
    this->newSize = size;
    this->parentRenderer = parentRenderer;
    this->parentFrameBuffer = parentRenderer->framebuffer;
    this->borderColor = Colors.gray;
    allowKeyboardDrawing = true;
    this->title = title;
    moveToFront = false;

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
    RemoveFromStack();
}


void Window::Free()
{
    AddToStack("Free", "WindowStuff/Window/window.cpp");
    free(framebuffer->BaseAddress);
    free(framebuffer);
    free(renderer);
    RemoveFromStack();
}

void Window::Render(Framebuffer* from, Framebuffer* to, Position pos, Size size, Window* window)
{
    AddToStack("Render", "WindowStuff/Window/window.cpp");
    uint32_t* pointerC = (uint32_t*)from->BaseAddress;
    for (int64_t y = 0; y < from->Height; y++)
    {
        int64_t newY = y + pos.y;
        uint32_t* pointerP = (uint32_t*)(to->BaseAddress + ((pos.x + (newY * to->Width)) * 4));
        for (int64_t x = 0; x < from->Width; x++)
        {
            int64_t newX = x + pos.x;
            if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height)
                if (*pointerC != *pointerP)
                    *pointerP = *pointerC;
                
            pointerC++;
            pointerP++;
        }
    }


    if (window != NULL)
    {
        {
            int64_t x = pos.x;
            int64_t y = pos.y- 21;
            window->parentRenderer->Clear(x,y,pos.x + size.width-1, pos.y-2, Colors.dgray);

            const char* stitle = StrSubstr(title, 0, size.width / 10);


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
            free((void*)stitle);
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
    
    RemoveFromStack();
}

/*
void Window::Render(Framebuffer* from, Framebuffer* to, Position pos, Size size)
{
    AddToStack("Render", "WindowStuff/Window/window.cpp"));
    uint32_t* pointerC = (uint32_t*)framebuffer->BaseAddress;
    for (int64_t y = 0; y < framebuffer->Height; y++)
    {
        int64_t newY = y + position.y;
        uint32_t* pointerP = (uint32_t*)(parentFrameBuffer->BaseAddress + ((position.x + (newY * parentFrameBuffer->Width)) * 4));
        for (int64_t x = 0; x < framebuffer->Width; x++)
        {
            int64_t newX = x + position.x;
            if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height)
                if (*pointerC != *pointerP)
                    *pointerP = *pointerC;
                
            pointerC++;
            pointerP++;
        }
    }


    {
        int64_t x = position.x;
        int64_t y = position.y- 21;
        parentRenderer->Clear(x,y,position.x + size.width-1,position.y-2, Colors.dgray);

        const char* stitle = StrSubstr(title, 0, size.width / 10);
        if (activeWindow == this)
            parentRenderer->putStr(stitle, x, y, Colors.white);
        else
            parentRenderer->putStr(stitle, x, y, Colors.bgray);
        free((void*)stitle);
    }
    

    uint32_t cBorder = borderColor;
    if (activeWindow == this)
        cBorder = Colors.bgreen;

    uint8_t counter = 0;
    for (int64_t x = -1; x < size.width + 1; x++)
    {
        int64_t newX = x + position.x;
        int64_t newY = -1 + position.y;
        if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height && (counter % 2) == 1)
            *(uint32_t*)(parentFrameBuffer->BaseAddress + ((newX + (newY * parentFrameBuffer->Width)) * 4)) = cBorder;
        
        
        newY = size.height + position.y;
        if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height && (counter % 2) == 0)
            *(uint32_t*)(parentFrameBuffer->BaseAddress + ((newX + (newY * parentFrameBuffer->Width)) * 4)) = cBorder;

        newY = -22 + position.y;
        if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height && (counter % 2) == 0)
            *(uint32_t*)(parentFrameBuffer->BaseAddress + ((newX + (newY * parentFrameBuffer->Width)) * 4)) = cBorder;
    
        counter++;
    }

    counter = 0;
    for (int64_t y = -22; y < size.height; y++)
    {
        int64_t newX = size.width + position.x;
        int64_t newY = y + position.y;
        if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height && (counter % 2) == 1)
            *(uint32_t*)(parentFrameBuffer->BaseAddress + ((newX + (newY * parentFrameBuffer->Width)) * 4)) = cBorder;
        
        newX = -1 + position.x;
        if (newX >= 0 && newY >= 0 && newX < parentFrameBuffer->Width && newY < parentFrameBuffer->Height && (counter % 2) == 0)
            *(uint32_t*)(parentFrameBuffer->BaseAddress + ((newX + (newY * parentFrameBuffer->Width)) * 4)) = cBorder;
        
        counter++;
    } 
    
    RemoveFromStack();
}

*/


void Window::Render()
{
    Render(framebuffer, parentFrameBuffer, position, size, this);
}


void Window::Resize(Size newSize)
{
    AddToStack("Resize", "WindowStuff/Window/window.cpp");
    if (newSize.height < 10)
        newSize.height = 10;

    if (newSize.width < 10)
        newSize.width = 10;

    this->newSize = newSize;

    {
        Framebuffer* oldFramebuffer = framebuffer;
        BasicRenderer* oldRenderer = renderer;
        {
            framebuffer = (Framebuffer*)malloc(sizeof(Framebuffer));
            *framebuffer = Framebuffer();
            framebuffer->Height = newSize.height;
            framebuffer->Width = newSize.width;
            framebuffer->PixelsPerScanLine = newSize.width;
            framebuffer->BufferSize = newSize.height * newSize.width * 4;
            framebuffer->BaseAddress = malloc(framebuffer->BufferSize);
        }

        {
            renderer = (BasicRenderer*)malloc(sizeof(Framebuffer));
            *renderer = BasicRenderer(framebuffer, GlobalRenderer->psf1_font);
        }

        renderer->Clear(Colors.black);
        renderer->CursorPosition = oldRenderer->CursorPosition;
        Render(oldFramebuffer, framebuffer, Position(0, 0), size, NULL);

        free((void*)oldFramebuffer->BaseAddress);
        free((void*)oldFramebuffer);
        free((void*)oldRenderer);
    }
    size = newSize;
    RemoveFromStack();
}

void CopyFrameBuffer(Framebuffer* a, Framebuffer* b)
{
    AddToStack("CopyFrameBuffer", "WindowStuff/Window/window.cpp");
    uint32_t* endAddr = (uint32_t*)(a->BaseAddress + a->BufferSize);

    uint32_t* addrB = (uint32_t*)b->BaseAddress;
    for (uint32_t* addrA = (uint32_t*)a->BaseAddress; addrA < endAddr; addrA++, addrB++)
        *addrB = *addrA; 
    RemoveFromStack();
}

void CopyFrameBuffer(Framebuffer* a, Framebuffer* a2, Framebuffer* b)
{
    AddToStack("CopyFrameBuffer", "WindowStuff/Window/window.cpp");
    uint32_t* endAddr = (uint32_t*)(a->BaseAddress + a->BufferSize);

    uint32_t* addrA2 = (uint32_t*)a2->BaseAddress;
    uint32_t* addrB = (uint32_t*)b->BaseAddress;
    //uint64_t counter = 0;

    for (uint32_t* addrA = (uint32_t*)a->BaseAddress; addrA < endAddr; addrA++, addrA2++, addrB++)
        if (*addrA2 != *addrA)
        {
            *addrB = *addrA;//Colors.bred;//*addrA; 
            *addrA2 = *addrA; 
            //counter++;
        }
        //else
        //    *addrB = *addrA;

    //free((void*)osData.mainTerminalWindow->title);
    //osData.mainTerminalWindow->title = StrCopy(to_string(counter));
    RemoveFromStack();
}

void Window::Log(const char* message, const char* var, uint32_t col)
{
    renderer->Print("> ");
    renderer->Println(message, var, col);
}

void Window::Log(const char* message)
{
    Log(message, NULL, renderer->color);
}

void Window::Log(const char* message, const char* var)
{
    Log(message, var, renderer->color);
}

void Window::Log(const char* message, uint32_t col)
{
    Log(message, NULL, col);
}
