#include "customInstance.h"

CustomInstance::CustomInstance(InstanceType type)
{
    this->instanceType = type;
    FreeFunc = (void(*)(void*))&Free;
}

void CustomInstance::Free()
{

}