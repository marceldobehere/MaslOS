#pragma once
#include "../SubInstances/defaultInstance/defaultInstance.h"
#include "../SubInstances/terminalInstance/terminalInstance.h"
#include "../../BasicRenderer.h"

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
};

class Window
{
    private:
    BasicRenderer* parentRenderer;
    
    public:
    DefaultInstance* instance;
    bool allowKeyboardDrawing;
    Size size;
    Position position;
    Size newSize;
    Position newPosition;
    BasicRenderer* renderer;
    Framebuffer* framebuffer;
    Framebuffer* parentFrameBuffer;
    uint32_t borderColor;
    bool moveToFront;
    const char* title;
    Window(DefaultInstance* instance, Size size, Position position, BasicRenderer* parentRenderer, const char* title);

    void Render();
    void Free();
};


extern Window* activeWindow;