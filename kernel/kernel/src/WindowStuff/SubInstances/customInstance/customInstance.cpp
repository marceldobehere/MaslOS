#include "customInstance.h"

CustomInstance::CustomInstance(InstanceType type)
{
    this->instanceType = type;
    FreeFuncHelp = (void*)this;
    FreeFunc = (void(*)(void*))&Free;
}

void CustomInstance::Free()
{

}