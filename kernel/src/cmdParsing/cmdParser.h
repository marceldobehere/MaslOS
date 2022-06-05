#pragma once
#include <stdint.h>

struct StringArrData
{
    char** data;
    int len;
    void* addrOfData;
};

struct ParsedColData
{
    bool parseSuccess;
    uint32_t col;
};

StringArrData* SplitLine(char* input);

void ParseCommand(char* input);

void LogError(const char* msg);

void LogError(const char* msg, const char* var);

void LogInvalidArgumentCount(int expected, int found);

void SetCmd(const char* name, const char* val);

void GetCmd(const char* name);

ParsedColData ParseColor(const char* col);