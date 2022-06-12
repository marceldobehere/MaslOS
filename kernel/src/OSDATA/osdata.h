#pragma once
#include "../kernelUtil.h"
#include "../customClasses/list.h"
#include "../WindowStuff/Window/window.h"

struct OSData
{
    bool exit;
    KernelInfo* kernelInfo;
    List<Window*> windows;
    Window* realMainWindow;
    Window* realMainWindow2;
    Window* mainTerminalWindow;
};

extern OSData osData;