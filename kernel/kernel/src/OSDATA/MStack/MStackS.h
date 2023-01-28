#pragma once
#include "../../cmdParsing/cstrTools.h"

struct MStack
{
    const char* name;
    const char* filename;
    int line;

    MStack(const char* name, const char* filename, int line)
    {
        this->name = name;
        this->filename = filename;
        this->line = line;
    }

    MStack(const char* name, const char* filename)
    {
        this->name = name;
        this->filename = filename;
        line = 0;
    }

    MStack()
    {
        name = "<NONE>";
        filename = "<NONE>";
        line = -1;
    }

    bool operator==(MStack other)
    {
        return StrEquals(name, other.name) && StrEquals(filename, other.filename) && (line == other.line);
    }
};

