#include "window.h"
#include "../../memory/heap.h"
#include "../../cStdLib/cstrTools.h"
#include "../../osData/osData.h"

Window* activeWindow = NULL;

Window::Window(DefaultInstance* instance, Size size, Position position, const char* title, bool showTitleBar, bool showBorder, bool moveable)
{
    AddToStack();
    OnClose = NULL;
    OnResize = NULL;
    OnPartRender = NULL;
    OnCloseHelp = NULL;
    OnResizeHelp = NULL;
    OnPartRenderHelp = NULL;
    RenderWhenHidden = false;
    closeable = true;
    this->instance = instance;
    this->position = position;
    this->size = size;
    this->newPosition = position;
    this->newSize = size;
    this->hidden = false;
    this->resizeable = true;
    maximize = false;
    oldMaximize = false;
    oldPreMaxSize = size;
    oldPreMaxPosition = position;
    oldHidden = hidden;
    oldPreMaxBorder = showBorder;
    oldPreMaxTitle = showTitleBar;
    //this->parentRenderer = parentRenderer;
    //this->parentFrameBuffer = parentRenderer->framebuffer;
    //this->borderColor = Colors.gray;
    this->defaultBorderColor = Colors.gray;
    this->selectedBorderColor = Colors.bgreen;
    this->defaultTitleColor = Colors.gray;
    this->selectedTitleColor = Colors.white;
    this->defaultTitleBackgroundColor = 0xff555555;
    allowKeyboardDrawing = true;
    this->title = StrCopy(title);
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
    else if (instance->instanceType == InstanceType::GUI)
        icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.GUI_PGM];
    else if (instance->instanceType == InstanceType::TESTO_PGM)
        icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.TESTO_PGM];
    else if (instance->instanceType == InstanceType::WARNING)
        icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.WARNING];
    else if (instance->instanceType == InstanceType::CRASH)
        icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.CRASH];
    else
        icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.DEFAULT];


    {
        framebuffer = (Framebuffer*)_Malloc(sizeof(Framebuffer), "new Framebuffer (struct) for Window");
        *framebuffer = Framebuffer();
        framebuffer->Height = size.height;
        framebuffer->Width = size.width;
        framebuffer->PixelsPerScanLine = size.width;
        framebuffer->BufferSize = size.height * size.width * 4;
        if (framebuffer != NULL)
            framebuffer->BaseAddress = _Malloc(framebuffer->BufferSize, "New Framebuffer for Window");
    }
    
    {
        backbuffer = (Framebuffer*)_Malloc(sizeof(Framebuffer), "New Backbuffer (struct) for Window");
        *backbuffer = Framebuffer();
        backbuffer->Height = size.height;
        backbuffer->Width = size.width;
        backbuffer->PixelsPerScanLine = size.width;
        backbuffer->BufferSize = size.height * size.width * 4;
        if (backbuffer != NULL)
            backbuffer->BaseAddress = _Malloc(backbuffer->BufferSize, "New Backbuffer for Window");
    }

    {
        renderer = (BasicRenderer*)_Malloc(sizeof(BasicRenderer), "New Basic Renderer for Window");
        if (renderer != NULL)
            *renderer = BasicRenderer(framebuffer, GlobalRenderer->psf1_font);
    }
    
    {
        brenderer = (BasicRenderer*)_Malloc(sizeof(BasicRenderer), "New Basic Renderer for Backbuffer in Window");
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

    if (MousePosition.x >= x - 20 && MousePosition.x <= x && MousePosition.y >= y && MousePosition.y <= y + 20
        && closeable)
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

    AddToStack();
    if (OnClose != NULL)
        OnClose(OnCloseHelp, this);
    RemoveFromStack();

    AddToStack();
    if (instance != NULL)
        instance->DefaultFree();
    RemoveFromStack();


    _Free(framebuffer->BaseAddress);
    _Free(framebuffer);
    _Free(backbuffer->BaseAddress);
    _Free(backbuffer);
    _Free(renderer);
    _Free(brenderer);
    _Free(title);


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

void Window::RenderStuff()
{
    AddToStack();
    if (OnPartRender != NULL && (RenderWhenHidden || !hidden))
    {
        OnPartRender(OnPartRenderHelp, this);
    }

    RemoveFromStack();
}

void Window::Render(Framebuffer* from, Framebuffer* to, Position pos, Size size, Window* window)
{
    AddToStack();

    uint32_t* pointerC = (uint32_t*)from->BaseAddress;

    int64_t x1, y1, x2, y2;
    x1 = 0;
    y1 = 0;
    x2 = size.width - 1; 
    y2 = size.height - 1;

    if (y1 + pos.y < 0)
        y1 = -pos.y;
    if (x1 + pos.x < 0)
        x1 = -pos.x;
    
    if (y2 + pos.y >= to->Height)
        y2 = to->Height - 1 - pos.y;
    if (x2 + pos.x >= to->Width)
        x2 = to->Width - 1 - pos.x;




    for (int64_t y = y1; y <= y2; y++)
        for (int64_t x = x1; x <= x2; x++)
            *(uint32_t*)((uint64_t)to->BaseAddress + 4 * ((x + pos.x) + (y + pos.y) * to->PixelsPerScanLine)) = 
            *(uint32_t*)((uint64_t)from->BaseAddress + 4 * (x + y * from->PixelsPerScanLine));

    //RemoveFromStack();
    //return;


    uint64_t toBaseAddr = (uint64_t)to->BaseAddress;

    if (window != NULL)
    {
        if (true)
        {
            int64_t x = pos.x;
            int64_t y = pos.y- 21;
            GlobalRenderer->Clear(x,y,pos.x + size.width-1, pos.y-2, Colors.dgray);

            const char* stitle = title;//StrSubstr(title, 0, size.width / 10);

            GlobalRenderer->putStr(stitle, x, y, Colors.white);
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
                *(uint32_t*)(toBaseAddr + ((newX + (newY * to->PixelsPerScanLine)) * 4)) = cBorder;
            
            
            newY = size.height + pos.y;
            if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 0)
                *(uint32_t*)(toBaseAddr + ((newX + (newY * to->PixelsPerScanLine)) * 4)) = cBorder;

            newY = -22 + pos.y;
            if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 0)
                *(uint32_t*)(toBaseAddr + ((newX + (newY * to->PixelsPerScanLine)) * 4)) = cBorder;
        
            counter++;
        }

        counter = 0;
        for (int64_t y = -22; y < size.height; y++)
        {
            int64_t newX = size.width + pos.x;
            int64_t newY = y + pos.y;
            if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 1)
                *(uint32_t*)((uint64_t)toBaseAddr + ((newX + (newY * to->PixelsPerScanLine)) * 4)) = cBorder;
            
            newX = -1 + pos.x;
            if (newX >= 0 && newY >= 0 && newX < to->Width && newY < to->Height && (counter % 2) == 0)
                *(uint32_t*)((uint64_t)toBaseAddr + ((newX + (newY * to->PixelsPerScanLine)) * 4)) = cBorder;
            
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
    // if (!resizeable)
    //     return;
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
            framebuffer = (Framebuffer*)_Malloc(sizeof(Framebuffer), "New Framebuffer (struct) for Window (resize)");
            if (framebuffer != NULL)
            {
                *framebuffer = Framebuffer();
                framebuffer->Height = _newSize.height;
                framebuffer->Width = _newSize.width;
                framebuffer->PixelsPerScanLine = _newSize.width;
                framebuffer->BufferSize = _newSize.height * _newSize.width * 4;
                framebuffer->BaseAddress = _Malloc(framebuffer->BufferSize, "New Framebuffer for Window (resize)");
            }
        }
        
        {
            backbuffer = (Framebuffer*)_Malloc(sizeof(Framebuffer), "New Backbuffer (struct) for Window (resize)");
            if (backbuffer != NULL)
            {
                *backbuffer = Framebuffer();
                backbuffer->Height = _newSize.height;
                backbuffer->Width = _newSize.width;
                backbuffer->PixelsPerScanLine = _newSize.width;
                backbuffer->BufferSize = _newSize.height * _newSize.width * 4;
                backbuffer->BaseAddress = _Malloc(backbuffer->BufferSize, "New Backbuffer for Window (resize)");
            }
        }

        {
            renderer = (BasicRenderer*)_Malloc(sizeof(BasicRenderer), "New Basic Renderer for Framebuffer (resize)");
            if (renderer != NULL)
                *renderer = BasicRenderer(framebuffer, GlobalRenderer->psf1_font);
        }
        
        {
            brenderer = (BasicRenderer*)_Malloc(sizeof(BasicRenderer), "New Basic Renderer for Backbuffer (resize)");
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

            _Free((void*)oldFramebuffer->BaseAddress);
            _Free((void*)oldFramebuffer);
            _Free((void*)oldBackbuffer->BaseAddress);
            _Free((void*)oldBackbuffer);
            _Free((void*)oldRenderer);
            _Free((void*)oldBrenderer);
        }
    }
    this->newSize = _newSize;
    this->size = _newSize;

    if (OnResize != NULL)
        OnResize(OnResizeHelp, this);

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

Window* FindWindowWithId(int64_t id)
{
    for (int i = 0; i < osData.windows.GetCount(); i++)
        if (osData.windows.ElementAt(i)->windowId == id)
            return osData.windows.ElementAt(i);

    return NULL;
}

#include "../../tasks/closeWindow/taskWindowClose.h"

bool DeleteWindowWithId(int64_t id)
{
    Window* window = FindWindowWithId(id);
    if (window == NULL)
        return false;
    
    osData.osTasks.Add(NewWindowCloseTask(window));
    return true;
}

#include "../SubInstances/guiInstance/guiInstance.h"

bool CreateWindowWithId(int64_t id)
{
    Window* window = FindWindowWithId(id);
    if (window != NULL)
        return false;

    Window* _mainWindow = (Window*)_Malloc(sizeof(Window), "Window");
    GuiInstance* _gui = new GuiInstance(_mainWindow);
    *(_mainWindow) = Window((DefaultInstance*)_gui, Size(100, 100), Position(10, 40), "Window", true, true, true);
    _mainWindow->windowId = id;
    osData.windows.Add(_mainWindow);
    _gui->Init();

    osData.windowsToGetActive.Enqueue(_mainWindow);

    return true;
}


bool SetWindowAttribute(int64_t id, WindowAttributeType type, uint64_t val)
{
    Window* window = FindWindowWithId(id);
    if (window == NULL)
        return false;

    if (type == WindowAttributeType::POSITION_X)
    {
        window->newPosition.x = *((int32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::POSITION_Y)
    {
        window->newPosition.y = *((int32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::SIZE_X)
    {
        window->newSize.width = *((uint32_t*)&val);
        if (window->newSize.width < 50)
            window->newSize.width = 50;
        return true;
    }
    if (type == WindowAttributeType::SIZE_Y)
    {
        window->newSize.height = *((uint32_t*)&val);
        if (window->newSize.height < 50)
            window->newSize.height = 50;
        return true;
    }
    if (type == WindowAttributeType::ID)
    {
        window->windowId = *((int64_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::DEFAULT_BORDER_COLOR)
    {
        window->defaultBorderColor = *((uint32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::SELECTED_BORDER_COLOR)
    {
        window->selectedBorderColor = *((uint32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::DEFAULT_TITLE_COLOR)
    {
        window->defaultTitleColor = *((uint32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::SELECTED_TITLE_COLOR)
    {
        window->selectedTitleColor = *((uint32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::DEFAULT_TITLE_BACKGROUND_COLOR)
    {
        window->defaultTitleBackgroundColor = *((uint32_t*)&val);
        return true;
    }

    if (type == WindowAttributeType::IS_TITLEBAR_SHOWN)
    {
        window->showTitleBar = *((bool*)&val);
        return true;
    }
    if (type == WindowAttributeType::IS_BORDER_SHOWN)
    {
        window->showBorder = *((bool*)&val);
        return true;
    }
    if (type == WindowAttributeType::IS_WINDOW_MOVEABLE)
    {
        window->moveable = *((bool*)&val);
        return true;
    }
    if (type == WindowAttributeType::IS_WINDOW_SHOWN)
    {
        window->hidden = !*((bool*)&val);
        return true;
    }
    if (type == WindowAttributeType::IS_WINDOW_RESIZEABLE)
    {
        window->resizeable = *((bool*)&val);
        return true;
    }

    if (type == WindowAttributeType::TITLE_TEXT)
    {
        const char* nT = StrCopy(*((const char**)&val));
        _Free((void*)window->title);
        window->title = nT;
        
        return true;
    }

    return false;
}

int GetWindowAttributeSize(WindowAttributeType type)
{
    if (type == WindowAttributeType::POSITION_X)
        return 4;
    if (type == WindowAttributeType::POSITION_Y)
        return 4;
    if (type == WindowAttributeType::SIZE_X)
        return 4;
    if (type == WindowAttributeType::SIZE_Y)
        return 4;

    if (type == WindowAttributeType::ID)
        return 8;

    if (type == WindowAttributeType::DEFAULT_BORDER_COLOR)
        return 4;
    if (type == WindowAttributeType::SELECTED_BORDER_COLOR)
        return 4;
    if (type == WindowAttributeType::DEFAULT_TITLE_COLOR)
        return 4;
    if (type == WindowAttributeType::SELECTED_TITLE_COLOR)
        return 4;
    if (type == WindowAttributeType::DEFAULT_TITLE_BACKGROUND_COLOR)
        return 4;

    if (type == WindowAttributeType::IS_TITLEBAR_SHOWN)
        return 1;
    if (type == WindowAttributeType::IS_BORDER_SHOWN)
        return 1;
    if (type == WindowAttributeType::IS_WINDOW_MOVEABLE)
        return 1;
    if (type == WindowAttributeType::IS_WINDOW_SHOWN)
        return 1;
    if (type == WindowAttributeType::IS_WINDOW_RESIZEABLE)
        return 1;
    if (type == WindowAttributeType::IS_WINDOW_SELECTED)
        return 1;

    if (type == WindowAttributeType::TITLE_TEXT)
        return 8;

    return 0;
}

uint64_t GetWindowAttribute(int64_t id, WindowAttributeType type)
{
    Window* window = FindWindowWithId(id);
    if (window == NULL)
        return false;

    uint8_t temp[8];
    for (int i = 0; i < 8; i++)
        temp[i] = 0;

    if (type == WindowAttributeType::POSITION_X)
        *((int32_t*)temp) = window->position.x;
    if (type == WindowAttributeType::POSITION_Y)
        *((int32_t*)temp) = window->position.y;
    if (type == WindowAttributeType::SIZE_X)
        *((uint32_t*)temp) = window->size.width;
    if (type == WindowAttributeType::SIZE_Y)
        *((uint32_t*)temp) = window->size.height;

    if (type == WindowAttributeType::ID)
        *((int64_t*)temp) = window->windowId;

    if (type == WindowAttributeType::DEFAULT_BORDER_COLOR)
        *((uint32_t*)temp) = window->defaultBorderColor;
    if (type == WindowAttributeType::SELECTED_BORDER_COLOR)
        *((uint32_t*)temp) = window->selectedBorderColor;
    if (type == WindowAttributeType::DEFAULT_TITLE_COLOR)
        *((uint32_t*)temp) = window->defaultTitleColor;
    if (type == WindowAttributeType::SELECTED_TITLE_COLOR)
        *((uint32_t*)temp) = window->selectedTitleColor;
    if (type == WindowAttributeType::DEFAULT_TITLE_BACKGROUND_COLOR)
        *((uint32_t*)temp) = window->defaultTitleBackgroundColor;

    if (type == WindowAttributeType::IS_TITLEBAR_SHOWN)
        *((bool*)temp) = window->showTitleBar;
    if (type == WindowAttributeType::IS_BORDER_SHOWN)
        *((bool*)temp) = window->showBorder;
    if (type == WindowAttributeType::IS_WINDOW_MOVEABLE)
        *((bool*)temp) = window->moveable;
    if (type == WindowAttributeType::IS_WINDOW_SHOWN)
        *((bool*)temp) = !window->hidden;
    if (type == WindowAttributeType::IS_WINDOW_RESIZEABLE)
        *((bool*)temp) = window->resizeable;
    if (type == WindowAttributeType::IS_WINDOW_SELECTED)
        *((bool*)temp) = activeWindow == window;

    if (type == WindowAttributeType::TITLE_TEXT)
        *((uint64_t*)temp) = (uint64_t)window->title;


    return *((uint64_t*)temp);
}