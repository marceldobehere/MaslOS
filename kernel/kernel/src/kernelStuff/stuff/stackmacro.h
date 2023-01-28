#pragma once
#include "../../OSDATA/MStack/MStackM.h"

#define RECORD_STACK_TRACE true



#define AddToStack() AddToTheMStack(MStack(__PRETTY_FUNCTION__, __FILE__, __LINE__));

#define RemoveFromStack() RemoveTheLastElementFromTheMStack();


#if !RECORD_STACK_TRACE

    #undef AddToStack
    #define AddToStack() ;

    #undef RemoveFromStack
    #define RemoveFromStack() ;

#endif

