#include "defaultInstance.h"
#include "../terminalInstance/terminalInstance.h"
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
            free(this);
            break;
        }
    }
}