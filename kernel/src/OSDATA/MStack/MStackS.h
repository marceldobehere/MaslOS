#pragma once
#include "../../cmdParsing/cstrTools.h"

struct MStack
{
    const char* name;
    const char* filename;

    MStack(const char* name, const char* filename)
    {
        this->name = name;
        this->filename = filename;
    }

    MStack()
    {
        name = "<NONE>";
        filename = "<NONE>";
    }

    bool operator==(MStack other)
    {
        return StrEquals(name, other.name);
    }
};

