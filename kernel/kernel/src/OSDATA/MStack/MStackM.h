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

    #define BenchmarkStackSize 10000

    extern MStack BenchmarkStackArr1[];
    extern MStack BenchmarkStackArr2[];
    extern MStack BenchmarkStackArrSave[];

    extern int64_t BenchmarkStackPointer1;
    extern int64_t BenchmarkStackPointer2;
    extern int64_t BenchmarkStackPointerSave;
    extern bool BenchmarkEnabled;
    extern int BenchmarkMode;
}

// void AddToTheMStack(MStack thing);
// void RemoveTheLastElementFromTheMStack();

#include "../../interrupts/panic.h"
// #include "MStackBRO.h"

// void bleh(MStack a);

void AddToTheMStack(MStack thing);
// {
//     //bleh(thing);
//     //Panic("BRO", true);
//     //_BroAddTheBenchmarkIBegOfYou(thing);
//     // if (MStackData::BenchmarkEnabled)
//     //     _AddBenchmark(thing);

//     if (MStackData::stackPointer < 1000)
//     {
//         MStackData::stackArr[MStackData::stackPointer] = thing;
//         MStackData::stackPointer++;
//     }
//     else
//         Panic("Stack overflow", true);
// }

void RemoveTheLastElementFromTheMStack();
// {
//     // if (MStackData::BenchmarkEnabled)
//     //     _RemoveBenchmark(MStackData::stackArr[MStackData::stackPointer]);

//     if (MStackData::stackPointer > 0)
//         MStackData::stackPointer--;
//     else
//     {
//         MStackData::stackPointer = 900;
//         Panic("Stack underflow", true);
//     }
// }

//#include "../../kernelStuff/stuff/stackmacro.h"

void SaveBenchmarkStack(int mode);
