#pragma once

#include "MStackS.h"
#include "../../OSDATA/osdata.h"

struct stackframe
{
    struct stackframe* ebp;
    uint64_t eip;
};

void PrintMStackTrace(MStack stack[], int64_t size, BasicRenderer* renderer, uint32_t col);
void PrintMStackTrace(MStack stack[], int64_t size);

void AddToTheMStack(MStack thing);
void RemoveTheLastElementFromTheMStack();