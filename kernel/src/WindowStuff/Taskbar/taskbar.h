#pragma once
#include "../../OSDATA/osdata.h"
#include "../../customClasses/syncedList.h"

namespace Taskbar
{
    extern SyncedList<Window*>* taskWindowList;

    void InitTaskbar(kernelFiles::ImageFile* MButton, kernelFiles::ImageFile* mButtonS);

    void RenderTaskbar();




}