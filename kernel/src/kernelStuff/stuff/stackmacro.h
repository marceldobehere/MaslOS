#pragma once
#include "../../OSDATA/MStack/MStackM.h"

#define RECORD_STACK_TRACE true



#define AddToStack(name, location) AddToTheMStack(MStack(name, location));

#define RemoveFromStack() RemoveTheLastElementFromTheMStack();


#if !RECORD_STACK_TRACE

    #undef ADDToStack
    #define ADDToStack(name, location) ;

    #undef RemoveFromStack
    #define RemoveFromStack() ;

#endif

