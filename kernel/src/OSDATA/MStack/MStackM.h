#pragma once

#include "MStackS.h"
#include "../osdata.h"

struct stackframe
{
    struct stackframe* ebp;
    uint64_t eip;
};

void PrintMStackTrace(MStack stack[], int64_t size, BasicRenderer* renderer, uint32_t col);
void PrintMStackTrace(MStack stack[], int64_t size);

namespace MStackData
{
    extern MStack stackArr[];
    extern int64_t stackPointer;
}

// void AddToTheMStack(MStack thing);
// void RemoveTheLastElementFromTheMStack();

#include "../../interrupts/panic.h"

inline void AddToTheMStack(MStack thing)
{
    if (MStackData::stackPointer < 1000)
    {
        MStackData::stackArr[MStackData::stackPointer] = thing;
        MStackData::stackPointer++;
    }
    else
        Panic("Stack overflow");
}

inline void RemoveTheLastElementFromTheMStack()
{
    if (MStackData::stackPointer > 0)
        MStackData::stackPointer--;
    else
    {
        MStackData::stackPointer = 900;
        Panic("Stack underflow");
    }
}

//#include "../../kernelStuff/stuff/stackmacro.h"