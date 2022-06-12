#pragma once
#include <stdint.h>
#include "../OSDATA/userdata.h"
#include "../WindowStuff/Window/window.h"

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

void ParseCommand(char* input, char* oldInput, OSUser** user, Window* window);

void login(const char* name, const char* pass, OSUser** user, Window* window);

void login(const char* name, OSUser** user, Window* window);

void LogError(const char* msg, Window* window);

void LogError(const char* msg, const char* var, Window* window);

void LogInvalidArgumentCount(int expected, int found, Window* window);

void SetCmd(const char* name, const char* val, OSUser** user, Window* window);

void GetCmd(const char* name, OSUser* user, Window* window);

ParsedColData ParseColor(const char* col);