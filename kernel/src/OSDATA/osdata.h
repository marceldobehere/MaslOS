#pragma once
#include "../kernelUtil.h"
#include "../customClasses/list.h"
#include "../WindowStuff/Window/window.h"
#include "MStack/MStackS.h"


struct OSData
{
    bool exit;
    KernelInfo* kernelInfo;
    List<Window*> windows;
    Window* realMainWindow;
    Window* realMainWindow2;
    Window* mainTerminalWindow;
    MStack stackArr[1000];
    int64_t stackPointer = 0;
    bool drawBackground;
    bool enableStackTrace;
};

extern OSData osData;