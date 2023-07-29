#pragma once
#include <stddef.h>
#include <stdint.h>

struct SerialPixelUpdate
{
public:
    int x;
    int y;

    SerialPixelUpdate(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
};