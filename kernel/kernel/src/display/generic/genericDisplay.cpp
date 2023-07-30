#include "genericDisplay.h"
#include "../normalDisplay/normalDisplay.h"
#include "../serialManagerDisplay/serialManagerDisplay.h"

void GenericDisplay::StartFrame()
{
    if (type == DisplayType::NormalType)
        ((NormalDisplay*)this)->StartFrame();
    else if (type == DisplayType::SerialManagerType)
        ((SerialManagerDisplay*)this)->StartFrame();
}

void GenericDisplay::UpdatePixel(int x, int y)
{
    if (type == DisplayType::NormalType)
        ((NormalDisplay*)this)->UpdatePixel(x, y);
    else if (type == DisplayType::SerialManagerType)
        ((SerialManagerDisplay*)this)->UpdatePixel(x, y);
}

void GenericDisplay::EndFrame()
{
    if (type == DisplayType::NormalType)
        ((NormalDisplay*)this)->EndFrame();
    else if (type == DisplayType::SerialManagerType)
        ((SerialManagerDisplay*)this)->EndFrame();
}

void GenericDisplay::Free()
{
    if (type == DisplayType::NormalType)
        ((NormalDisplay*)this)->Free();
    else if (type == DisplayType::SerialManagerType)
        ((SerialManagerDisplay*)this)->Free();
}