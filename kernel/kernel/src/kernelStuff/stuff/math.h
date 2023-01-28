#pragma once

struct Point
{
    unsigned int x;
    unsigned int y;
};


struct MPoint
{
    int x;
    int y;
};

// inline int min(int a, int b);
// inline int max(int a, int b);

inline int min(int a, int b)
{
    if (a <= b)
        return a;
    return b;
}

inline int max(int a, int b)
{
    if (a >= b)
        return a;
    return b;
}