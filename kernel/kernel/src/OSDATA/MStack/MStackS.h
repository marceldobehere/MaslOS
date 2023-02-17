#pragma once
#include "../../cmdParsing/cstrTools.h"

struct MStack
{
    const char* name;
    const char* filename;
    int line;

    int layer;
    uint64_t time;
    bool close;

    MStack(const char* name, const char* filename, int line, int layer, uint64_t time, bool close)
    ;

    MStack(const char* name, const char* filename, int line)
    ;

    MStack(const char* name, const char* filename)
    ;

    MStack()
    ;

    bool operator==(MStack other)
    ;
};

