#pragma once
#include "../kernelUtil.h"

struct OSData
{
    bool exit;
    KernelInfo* kernelInfo;
};

extern OSData osData;