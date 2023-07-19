#include "debugTerminalInstance.h"


DebugTerminalInstance::DebugTerminalInstance(Window* window)
{
    this->window = window;
    instanceType = InstanceType::DebugTerminal;
    FreeFunc = (void(*)(void*))&Free;
}

void DebugTerminalInstance::Free()
{

}
