#pragma once
#include <stdint.h>

struct OSStats
{
    uint64_t frameStartTime = 0;
    uint64_t frameEndTime = 10;

    int32_t totalFrameTime = 10;

    int32_t totalTaskTime = 2;
    int32_t totalOsTaskTime = 2;
    int32_t windowUpdateTime = 2;
    int32_t totalRenderTime = 2;
    int32_t totalWindowUpdateTime = 2;
    int32_t totalIdleTime = 1;

    uint8_t testThing = 0;
    uint64_t lastKeyboardCall = 0;
    uint64_t lastMouseCall = 0;
};



extern OSStats osStats;