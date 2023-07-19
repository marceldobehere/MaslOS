#include "debugTerminalInstance.h"


DebugTerminalInstance::DebugTerminalInstance(Window* window)
{
    this->window = window;
    instanceType = InstanceType::DebugTerminal;
    FreeFuncHelp = (void*)this;
    FreeFunc = (void(*)(void*))&Free;
}

void DebugTerminalInstance::Free()
{
    
}
