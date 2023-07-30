#include "normalDisplay.h"
#include "../../osData/osData.h"

NormalDisplay::NormalDisplay(Framebuffer* framebuffer)
{
    this->framebuffer = framebuffer;
    type = DisplayType::NormalType;
}

void NormalDisplay::Free()
{
    if (framebuffer != GlobalRenderer->framebuffer)
        _Free(framebuffer);
}

void NormalDisplay::StartFrame()
{

}

void NormalDisplay::UpdatePixel(int x, int y)
{

}

void NormalDisplay::EndFrame()
{

}

