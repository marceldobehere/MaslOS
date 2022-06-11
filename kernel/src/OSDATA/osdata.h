#pragma once
#include "../kernelUtil.h"
#include "../customClasses/list.h"
#include "../WindowStuff/Window/window.h"

struct OSData
{
    bool exit;
    KernelInfo* kernelInfo;
    List<Window*> windows;
};

extern OSData osData;