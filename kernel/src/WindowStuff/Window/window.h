#pragma once
#include "../SubInstances/defaultInstance/defaultInstance.h"
//#include "../SubInstances/terminalInstance/terminalInstance.h"
#include "../../Rendering/BasicRenderer.h"

struct Size
{
    int width, height; 
    Size(int width, int height)
    {
        this->height = height;
        this->width = width;
    }
    Size()
    {
        this->height = 0;
        this->width = 0;
    }

    bool operator== (Size other)
    {
        return (height == other.height) && (width == other.width);
    }

    bool operator!= (Size other)
    {
        return (height != other.height) || (width != other.width);
    }
};

struct Position
{
    int x, y;
    Position(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
    Position()
    {
        this->x = 0;
        this->y = 0;
    }

    bool operator== (Position other)
    {
        return (x == other.x) && (y== other.y);
    }

    bool operator!= (Position other)
    {
        return (x != other.x) || (y != other.y);
    }
};

enum WindowActionEnum
{
    _NONE,
    CLOSE,
    MIN_MAX,
    HIDE,
};


class Window
{
    private:
    BasicRenderer* parentRenderer;
    
    public:
    DefaultInstance* instance;
    kernelFiles::ImageFile* icon;
    bool allowKeyboardDrawing;
    Size size;
    Position position;
    Size newSize;
    Position newPosition;
    BasicRenderer* renderer;
    BasicRenderer* brenderer;
    Framebuffer* framebuffer;
    Framebuffer* backbuffer;
    Framebuffer* parentFrameBuffer;
    uint32_t defaultBorderColor;
    uint32_t selectedBorderColor;
    uint32_t defaultTitleColor;
    uint32_t selectedTitleColor;
    uint32_t defaultTitleBackgroundColor;
    // uint32_t tempTime;
    // uint32_t cpuUsagePercent;
    bool showTitleBar, showBorder, moveable, hidden;
    bool oldHidden;
    
    bool moveToFront;
    const char* title;
    Window(DefaultInstance* instance, Size size, Position position, const char* title, bool showTitleBar, bool showBorder, bool moveable);
    
    void BlitBackbuffer();
    void Resize(Size _newSize);
    void RenderDeprecated();
    void Render(Framebuffer* from, Framebuffer* to, Position pos, Size size, Window* window);
    void Free();

    void Log(const char* message);
    void Log(const char* message, const char* var);

    void Log(const char* message, uint32_t col);
    void Log(const char* message, const char* var, uint32_t col);

    Position GetMousePosRelativeToWindow();

    WindowActionEnum GetCurrentAction();
};


extern Window* activeWindow;


void CopyFrameBuffer(Framebuffer* a, Framebuffer* b);

void CopyFrameBuffer(Framebuffer* a, Framebuffer* a2, Framebuffer* b);