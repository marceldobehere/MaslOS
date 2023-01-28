#include "defaultInstance.h"
#include "../terminalInstance/terminalInstance.h"
#include "../newTerminalInstance/newTerminalInstance.h"
#include "../connect4Instance/connect4Instance.h"
#include "../guiInstance/guiInstance.h"
#include "../../../memory/heap.h"


void DefaultInstance::DefaultFree()
{
    switch (this->instanceType)
    {
        case InstanceType::Default:
        {
            _Free(this);
            break;
        }
        case InstanceType::DebugTerminal:
        {
            _Free(this);
            break;
        }
        case InstanceType::Terminal:
        {
            ((TerminalInstance*)this)->Free();
            _Free(this);
            break;
        }
        case InstanceType::NewTerminal:
        {
            ((NewTerminalInstance*)this)->Free();
            _Free(this);
            break;
        }
        case InstanceType::Connect4:
        {
            ((Connect4Instance*)this)->Free();
            _Free(this);
            break;
        }
        case InstanceType::GUI:
        {
            ((GuiInstance*)this)->Free();
            _Free(this);
            break;
        }
        default: 
        {
            _Free(this);
            break;
        }
    }
}