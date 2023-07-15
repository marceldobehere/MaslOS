#pragma once
#include "../cStdLib/list.h"


namespace AudioDeviceStuff
{
    extern int currentRawAudioIndex;
    extern int rawAudioDiv; // 59.6591 
    extern bool needMoreData;
    extern Audio::AudioOutputDevice* pcSpk;
    extern bool currentState;

    void init();
    void play(int timeYes);
    void resetTest();
}