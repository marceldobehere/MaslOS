#pragma once
#include "../../osData/osData.h"
#include "../../customClasses/syncedList.h"

namespace Taskbar
{
    extern SyncedList<Window*>* taskWindowList;
    extern Window* activeTabWindow;
    extern bool MButtonSelected;

    void InitTaskbar(kernelFiles::ImageFile* MButton, kernelFiles::ImageFile* mButtonS);

    void RenderTaskbar();

    //Window* GetItemAtMousePosition();



}