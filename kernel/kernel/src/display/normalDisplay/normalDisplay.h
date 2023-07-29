#pragma once
#include "../generic/genericDisplay.h"

class NormalDisplay : public GenericDisplay
{
public:
    NormalDisplay(Framebuffer* framebuffer);
    void StartFrame();
    void UpdatePixel(int x, int y);
    void EndFrame();
};