#include "cstrTools.h"
#include "../paging/PageFrameAllocator.h"

bool StrEquals(const char* a, const char* b)
{
    int index = 0;
    while (!(a[index] == 0 && b[index] == 0))
    {
        if (a[index] != b[index])
            return false;
        index++;
    }

    return true;
}

char* StrCopy(const char* og)
{
    char* newStr = (char*)GlobalAllocator->RequestPage();
    for (int i = 0; og[i] != 0; i++)
        newStr[i] = og[i];
    return newStr;
}