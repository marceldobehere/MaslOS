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
    DefaultInstance* instance;

    public:
    Size size;
    Position position;
    BasicRenderer* renderer;
    Framebuffer* framebuffer;
    Framebuffer* parentFrameBuffer;
    uint32_t borderColor;
    Window(DefaultInstance* instance, Size size, Position position, Framebuffer* parentFrameBuffer);

    void Render();
    void Free();
};


extern Window* activeWindow;