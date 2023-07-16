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

    MPoint(int x, int y);
    MPoint();

    bool operator==(const MPoint& other);
    bool operator!=(const MPoint& other);
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

inline int abs(int val)
{
    if (val < 0)
        return -val;
    return val;
}