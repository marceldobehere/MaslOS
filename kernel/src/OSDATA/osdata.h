#pragma once
#include "MStack/MStackS.h"

#include "../kernelStuff/stuff/kernelUtil.h"
//#include "../customClasses/list.h"

#include "../WindowStuff/WindowManager/windowManager.h"

//#include "../WindowStuff/Window/window.h"


struct OSData
{
    bool exit;
    bool booting;
    KernelInfo* kernelInfo;
    List<Window*> windows;
    List<Task*> osTasks;
    kernelFiles::ZIPFile* windowButtonZIP;
    kernelFiles::ZIPFile* windowIconZIP;
    //Window* realMainWindow;
    //Window* realMainWindow2;
    Window* mainTerminalWindow;
    Window* debugTerminalWindow;
    bool showDebugterminal;
    MStack stackArr[1000];
    int64_t stackPointer = 0;
    bool drawBackground;
    bool enableStackTrace;
    int64_t crashCount = 0;
    WindowManager::WindowPointerBufferThing* windowPointerThing;
};

extern OSData osData;