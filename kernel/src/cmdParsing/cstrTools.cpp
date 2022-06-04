#include "cstrTools.h"

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


