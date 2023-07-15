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

enum BuiltinCommand {
    Command_Help,
    Command_Clear,
    Command_BenchMarkReset,
    Command_Malloc,
    Command_Explorer,
    Command_NotePad,
    Command_Image,
    Command_MusicPlayer,
    Command_Doom,
    Command_MusicTest,
    Command_AC97Reset,
    Command_Tetris,
    Command_HeapCheck,
    Command_ShutDown,
    Command_Exit,
    Command_ConnectFour,
    Command_TaskManager,
    Command_DebugViewer,
    Command_Crash,
    Command_Crash2,
    Command_Crash3,
    Command_Crash4,
    Command_RenderLoop,
    Command_Invalid,
    Command_ResetDefaultSpeaker
};
BuiltinCommand BuiltinCommandFromStr(char* i);

StringArrData* SplitLine(const char* input);

void ParseCommand(char* input, char* oldInput, OSUser** user, Window* window);

void login(const char* name, const char* pass, OSUser** user, Window* window);

void login(const char* name, OSUser** user, Window* window);

void LogError(const char* msg, Window* window);

void LogError(const char* msg, const char* var, Window* window);

void LogInvalidArgumentCount(int expected, int found, Window* window);

void SetCmd(const char* name, const char* val, OSUser** user, Window* window);

void GetCmd(const char* name, OSUser* user, Window* window);

void HelpCommand(Window* window);

ParsedColData ParseColor(const char* col);
