#pragma once
#include "MStack/MStackS.h"

//#include "../kernelStuff/stuff/stackmacro.h"
#include "../kernelStuff/stuff/kernelUtil.h"
//#include "../customClasses/list.h"


#include "../WindowStuff/WindowManager/windowManager.h"

//#include "../WindowStuff/Window/window.h"
//#include "../kernelStuff/Disk_Stuff/Disk_Interfaces/generic/genericDiskInterface.h"

#define LIST_INCLUDE_GENERIC_DISK_INTERFACE
#define LIST_INCLUDE_TASK
#define LIST_INCLUDE_WINDOW
#include "../customClasses/list.h"
#undef LIST_INCLUDE_WINDOW
#undef LIST_INCLUDE_TASK
#undef LIST_INCLUDE_GENERIC_DISK_INTERFACE

struct OSData
{
    bool exit;
    bool booting;
    bool verboseBoot;
    KernelInfo* kernelInfo;
    List<Window*> windows;
    List<Window*> windowsToGetActive;
    List<Task*> osTasks;
    kernelFiles::ZIPFile* windowButtonZIP;
    kernelFiles::ZIPFile* windowIconZIP;
    //Window* realMainWindow;
    //Window* realMainWindow2;
    //Window* mainTerminalWindow;
    Window* debugTerminalWindow;
    Window* startMenuWindow;
    Window* activeCrashWindow;
    Window* preCrashWindow;
    bool showDebugterminal;
    bool NO_INTERRUPTS = false;
    // MStack stackArr[1000];
    // int64_t stackPointer = 0;
    bool drawBackground;
    bool enableStackTrace;
    int64_t crashCount = 0;
    int maxNonFatalCrashCount = 0;
    bool crashed = false;
    bool tempCrash = false;
    bool bgTaskRun = true;
    WindowManager::WindowPointerBufferThing* windowPointerThing;

    List<DiskInterface::GenericDiskInterface*> diskInterfaces;

    double mouseSensitivity = 1;

    uint8_t port64Val;

    uint32_t wantedFps = 60;

};



extern OSData osData;