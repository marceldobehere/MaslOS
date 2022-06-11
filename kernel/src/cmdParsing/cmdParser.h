#pragma once
#include <stdint.h>
#include "../OSDATA/userdata.h"

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

void ParseCommand(char* input, char* oldInput, OSUser** user);

void login(const char* name, const char* pass, OSUser** user);

void login(const char* name, OSUser** user);

void LogError(const char* msg);

void LogError(const char* msg, const char* var);

void LogInvalidArgumentCount(int expected, int found);

void SetCmd(const char* name, const char* val, OSUser** user);

void GetCmd(const char* name, OSUser* user);

ParsedColData ParseColor(const char* col);