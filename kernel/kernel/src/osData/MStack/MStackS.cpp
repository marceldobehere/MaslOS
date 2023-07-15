#include "MStackS.h"

MStack::MStack(const char* name, const char* filename, int line, int layer, uint64_t time, bool close)
{
    this->name = name;
    this->filename = filename;
    this->line = line;
    this->layer = layer;
    this->time = time;
    this->close = close;
}

MStack::MStack(const char* name, const char* filename, int line)
{
    this->name = name;
    this->filename = filename;
    this->line = line;
}

MStack::MStack(const char* name, const char* filename)
{
    this->name = name;
    this->filename = filename;
    line = 0;
}

MStack::MStack()
{
    name = "<NONE>";
    filename = "<NONE>";
    line = -1;

    layer = 0;
    time = 0;
    close = false;
}

bool MStack::operator==(MStack other)
{
    return StrEquals(name, other.name) && StrEquals(filename, other.filename) && (line == other.line)
    && (time == other.time) && (layer == other.layer) && (close == other.close);
}

