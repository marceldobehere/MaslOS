#include "defaultInstance.h"
#include "../terminalInstance/terminalInstance.h"
#include "../newTerminalInstance/newTerminalInstance.h"
#include "../../../memory/heap.h"


void DefaultInstance::DefaultFree()
{
    switch (this->instanceType)
    {
        case InstanceType::Default:
        {
            free(this);
            break;
        }
        case InstanceType::DebugTerminal:
        {
            free(this);
            break;
        }
        case InstanceType::Terminal:
        {
            ((TerminalInstance*)this)->Free();
            free(this);
            break;
        }
        case InstanceType::NewTerminal:
        {
            ((NewTerminalInstance*)this)->Free();
            free(this);
            break;
        }
        default: 
        {
            free(this);
            break;
        }
    }
}