#pragma once

#include "../Cols.h"

struct OSUserColData
{
    uint32_t userColor;
    uint32_t defaultTextColor;
    uint32_t arrowColor;
};

struct OSUser
{
    const char* userName;
    OSUserColData colData;
    uint32_t permLevel;
};


extern OSUser admin;
