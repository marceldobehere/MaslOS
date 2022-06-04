#pragma once

struct StringArrData
{
    char** data;
    int len;
    void* addrOfData;
};

StringArrData* SplitLine(char* input);

void ParseCommand(char* input);