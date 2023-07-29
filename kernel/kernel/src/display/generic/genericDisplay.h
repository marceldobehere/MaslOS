#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../../rendering/Framebuffer.h"

enum DisplayType
{
    NormalType,
    SerialManagerType
};

class GenericDisplay
{
public:
    DisplayType type;
    Framebuffer* framebuffer;
    void StartFrame();
    void UpdatePixel(int x, int y);
    void EndFrame();
};