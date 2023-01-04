#include "window.h"
#include "../../memory/heap.h"
#include "../../cmdParsing/cstrTools.h"
#include "../../OSDATA/osdata.h"

Window* activeWindow = NULL;

Window::Window(DefaultInstance* instance, Size size, Position position, const char* title, bool showTitleBar, bool showBorder, bool moveable)
{
    AddToStack();
    this->instance = instance;
    this->position = position;
    this->size = size;
    this->newPosition = position;
    this->newSize = size;
    this->hidden = false;
    oldHidden = hidden;
    //this->parentRenderer = parentRenderer;
    //this->parentFrameBuffer = parentRenderer->framebuffer;
    //this->borderColor = Colors.gray;
    this->defaultBorderColor = Colors.gray;
    this->selectedBorderColor = Colors.bgreen;
    this->defaultTitleColor = Colors.gray;
    this->selectedTitleColor = Colors.white;
    this->defaultTitleBackgroundColor = 0xff555555;
    allowKeyboardDrawing = true;
    this->title = title;
    moveToFront = false;
    this->showBorder = showBorder;
    this->showTitleBar = showTitleBar;
    this->moveable = moveable;
    
    if (instance == NULL)
        icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.DEFAULT];
    else if (instance->instanceType == InstanceType::Default)
        icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.DEFAULT];
    else if (instance->instanceType == InstanceType::Terminal)
        icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.TERMINAL];
    else if (instance->instanceType == InstanceType::NewTerminal)
        icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.TERMINAL];
    else if (instance->instanceType == InstanceType::DebugTerminal)
        icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.DEBUG];
    else
        icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.DEFAULT];


    {
        framebuffer = (Framebuffer*)malloc(sizeof(Framebuffer), "new Framebuffer (struct) for Window");
        *framebuffer = Framebuffer();
        framebuffer->Height = size.height;
        framebuffer->Width = size.width;
        framebuffer->PixelsPerScanLine = size.width;
        framebuffer->BufferSize = size.height * size.width * 4;
        if (framebuffer != NULL)
            framebuffer->BaseAddress = malloc(framebuffer->BufferSize, "New Framebuffer for Window");
    }
    
    {
        backbuffer = (Framebuffer*)malloc(sizeof(Framebuffer), "New Backbuffer (struct) for Window");
        *backbuffer = Framebuffer();
        backbuffer->Height = size.height;
        backbuffer->Width = size.width;
        backbuffer->PixelsPerScanLine = size.width;
        backbuffer->BufferSize = size.height * size.width * 4;
        if (backbuffer != NULL)
            backbuffer->BaseAddress = malloc(backbuffer->BufferSize, "New Backbuffer for Window");
    }

    {
        renderer = (BasicRenderer*)malloc(sizeof(BasicRenderer), "New Basic Renderer for Window");
        if (renderer != NULL)
            *renderer = BasicRenderer(framebuffer, GlobalRenderer->psf1_font);
    }
    
    {
        brenderer = (BasicRenderer*)malloc(sizeof(BasicRenderer), "New Basic Renderer for Backbuffer in Window");
        if (brenderer != NULL)
            *brenderer = BasicRenderer(backbuffer, GlobalRenderer->psf1_font);
    }
    RemoveFromStack();
}

Position Window::GetMousePosRelativeToWindow()
{
    return Position(MousePosition.x - position.x, MousePosition.y - position.y);
}

WindowActionEnum Window::GetCurrentAction()
{
    if (hidden)
        return WindowActionEnum::_NONE;

    int64_t x = position.x + size.width;
    int64_t y = position.y - 22;

    if (MousePosition.x >= x - 20 && MousePosition.x <= x && MousePosition.y >= y && MousePosition.y <= y + 20)
        return WindowActionEnum::CLOSE;
    x -= 20;

    if (MousePosition.x >= x - 20 && MousePosition.x <= x && MousePosition.y >= y && MousePosition.y <= y + 20)
        return WindowActionEnum::MIN_MAX;
    x -= 20;

    if (MousePosition.x >= x - 20 && MousePosition.x <= x && MousePosition.y >= y && MousePosition.y <= y + 20)
        return WindowActionEnum::HIDE;
    x -= 20;
    
    return WindowActionEnum::_NONE;
}

void Window::Free()
{
    AddToStack();
    free(framebuffer->BaseAddress);
    free(framebuffer);
    free(backbuffer->BaseAddress);
    free(backbuffer);
    free(renderer);
    free(brenderer);
    if (instance != NULL)
    {
        instance->DefaultFree();
    }
    RemoveFromStack();
}

void Window::BlitBackbuffer()
{
    uint32_t* fb = (uint32_t*)framebuffer->BaseAddress;
    uint32_t* bbe = (uint32_t*)((uint64_t)backbuffer->BaseAddress + backbuffer->BufferSize);
    for (uint32_t* bb = (uint32_t*)backbuffer->BaseAddress; bb < bbe;)
    {
        *fb = *bb;
        fb++;
        bb++;
    }
}

void Window::Render(Framebuffer* from, Framebuffer* to, Position pos, Size size, Window* window)
{
    AddToStack();
    uint32_t* pointerC = (uint32_t*)from->BaseAddress;
    for (int64_t y = 0; y < from->Height; y++)
    {
        int64_t newY = y + pos.y;
        uint32_t* pointerP = (uint32_t*)((uint64_t)to->BaseAddress + ((pos.x + (newY * to->Width)) * 4));
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

    uint64_t toBaseAddr = (uint64_t)to->BaseAddress;

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
        

        uint32_t cBorder = window->defaultBorderColor;
        if (activeWindow == window)
            cBorder = Colors.bgreen;

        uint8_t counter = 0;
        for (int64_t x = -1; x < size.width + 1; x++)
        {
            int64_t newX = x + pos.x;
            int64_t newY = -1 + pos.y;
            if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 1)
                *(uint32_t*)(toBaseAddr + ((newX + (newY * to->Width)) * 4)) = cBorder;
            
            
            newY = size.height + pos.y;
            if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 0)
                *(uint32_t*)(toBaseAddr + ((newX + (newY * to->Width)) * 4)) = cBorder;

            newY = -22 + pos.y;
            if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 0)
                *(uint32_t*)(toBaseAddr + ((newX + (newY * to->Width)) * 4)) = cBorder;
        
            counter++;
        }

        counter = 0;
        for (int64_t y = -22; y < size.height; y++)
        {
            int64_t newX = size.width + pos.x;
            int64_t newY = y + pos.y;
            if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 1)
                *(uint32_t*)((uint64_t)toBaseAddr + ((newX + (newY * to->Width)) * 4)) = cBorder;
            
            newX = -1 + pos.x;
            if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 0)
                *(uint32_t*)((uint64_t)toBaseAddr + ((newX + (newY * to->Width)) * 4)) = cBorder;
            
            counter++;
        } 
    }
    
    RemoveFromStack();
}

/*
void Window::Render(Framebuffer* from, Framebuffer* to, Position pos, Size size)
{
    AddToStack();
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


void Window::RenderDeprecated()
{
    Render(framebuffer, parentFrameBuffer, position, size, this);
}


void Window::Resize(Size _newSize)
{
    AddToStack();
    if (_newSize.height < 20)
        _newSize.height = 20;

    if (_newSize.width < 80)
        _newSize.width = 80;

    //this->newSize = _newSize;

    {
        Framebuffer* oldFramebuffer = framebuffer;
        Framebuffer* oldBackbuffer = backbuffer;
        BasicRenderer* oldRenderer = renderer;
        BasicRenderer* oldBrenderer = brenderer;

        {
            framebuffer = (Framebuffer*)malloc(sizeof(Framebuffer), "New Framebuffer (struct) for Window (resize)");
            if (framebuffer != NULL)
            {
                *framebuffer = Framebuffer();
                framebuffer->Height = _newSize.height;
                framebuffer->Width = _newSize.width;
                framebuffer->PixelsPerScanLine = _newSize.width;
                framebuffer->BufferSize = _newSize.height * _newSize.width * 4;
                framebuffer->BaseAddress = malloc(framebuffer->BufferSize, "New Framebuffer for Window (resize)");
            }
        }
        
        {
            backbuffer = (Framebuffer*)malloc(sizeof(Framebuffer), "New Backbuffer (struct) for Window (resize)");
            if (backbuffer != NULL)
            {
                *backbuffer = Framebuffer();
                backbuffer->Height = _newSize.height;
                backbuffer->Width = _newSize.width;
                backbuffer->PixelsPerScanLine = _newSize.width;
                backbuffer->BufferSize = _newSize.height * _newSize.width * 4;
                backbuffer->BaseAddress = malloc(backbuffer->BufferSize, "New Backbuffer for Window (resize)");
            }
        }

        {
            renderer = (BasicRenderer*)malloc(sizeof(BasicRenderer), "New Basic Renderer for Framebuffer (resize)");
            if (renderer != NULL)
                *renderer = BasicRenderer(framebuffer, GlobalRenderer->psf1_font);
        }
        
        {
            brenderer = (BasicRenderer*)malloc(sizeof(BasicRenderer), "New Basic Renderer for Backbuffer (resize)");
            if (brenderer != NULL)
                *brenderer = BasicRenderer(backbuffer, GlobalRenderer->psf1_font);
        }

        if (framebuffer == NULL || backbuffer == NULL || renderer == NULL || brenderer == NULL || framebuffer->BaseAddress == NULL || backbuffer->BaseAddress == NULL)
        {
            // Dont resize lmao
            _newSize = this->size;
            framebuffer = oldFramebuffer;
            backbuffer = oldBackbuffer;
            renderer = oldRenderer;
            brenderer = oldBrenderer;
        }
        else
        {
            renderer->Clear(Colors.black);
            renderer->CursorPosition = oldRenderer->CursorPosition;
            Render(oldFramebuffer, framebuffer, Position(0, 0), size, NULL);

            free((void*)oldFramebuffer->BaseAddress);
            free((void*)oldFramebuffer);
            free((void*)oldBackbuffer->BaseAddress);
            free((void*)oldBackbuffer);
            free((void*)oldRenderer);
            free((void*)oldBrenderer);
        }
    }
    this->newSize = _newSize;
    this->size = _newSize;
    RemoveFromStack();
}

void CopyFrameBuffer(Framebuffer* a, Framebuffer* b)
{
    AddToStack();
    uint32_t* endAddr = (uint32_t*)((uint64_t)a->BaseAddress + a->BufferSize);

    uint32_t* addrB = (uint32_t*)b->BaseAddress;
    for (uint32_t* addrA = (uint32_t*)a->BaseAddress; addrA < endAddr; addrA++, addrB++)
        *addrB = *addrA; 
    RemoveFromStack();
}

void CopyFrameBuffer(Framebuffer* a, Framebuffer* a2, Framebuffer* b)
{
    AddToStack();
    uint32_t* endAddr = (uint32_t*)((uint64_t)a->BaseAddress + a->BufferSize);

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
