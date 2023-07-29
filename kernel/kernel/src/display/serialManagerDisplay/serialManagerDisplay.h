#pragma once
#include "../generic/genericDisplay.h"
#include "../../cStdLib/list/list_serialPixelUpdate.h"
#include "../../serialManager/serialManager.h"

class SerialManagerDisplay : public GenericDisplay
{
public:
    List<SerialPixelUpdate>* pixelUpdates;
    SerialManager::Manager* manager;
    SerialManagerDisplay(SerialManager::Manager* manager, Framebuffer* framebuffer);
    void StartFrame();
    void UpdatePixel(int x, int y);
    void EndFrame();
};