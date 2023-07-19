#pragma once
#include <stddef.h>

enum InstanceType
{
    Default,
    Terminal,
    DebugTerminal,
    NewTerminal,
    Connect4,
    GUI,
    CRASH,
    WARNING,
    TESTO_PGM
};

class DefaultInstance
{
    protected:

    public:
        InstanceType instanceType = InstanceType::Default;
        void* audioSource = NULL;
        void* FreeFuncHelp = NULL;
        void (*FreeFunc)(void* bruh) = NULL;
        void DefaultFree();
};