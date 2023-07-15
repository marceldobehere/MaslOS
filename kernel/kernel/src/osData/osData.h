#pragma once
#include "MStack/MStackS.h"

//#include "../kernelStuff/stuff/stackmacro.h"
#include "../kernelStuff/stuff/kernelUtil.h"
//#include "../customClasses/list.h"


#include "../WindowStuff/WindowManager/windowManager.h"


#include "../customClasses/list.h"

#include "../devices/ac97/ac97.h"

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
    //List<Audio::BasicAudioDestination*> audioDestinations;

    List<Audio::AudioInputDevice*> audioInputDevices;
    List<Audio::AudioOutputDevice*> audioOutputDevices;

    Audio::AudioInputDevice* defaultAudioInputDevice = NULL;
    Audio::AudioOutputDevice* defaultAudioOutputDevice = NULL;

    double mouseSensitivity = 1;

    uint8_t port64Val;

    uint32_t wantedFps = 60;
    AC97::AC97Driver* ac97Driver = NULL;
    Audio::AudioOutputDevice* pcSpeakerDev;

};



extern OSData osData;