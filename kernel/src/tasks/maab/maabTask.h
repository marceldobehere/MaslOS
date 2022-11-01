#pragma once
#include "../task.h"
#include "../../WindowStuff/Window/window.h"
#include "../../WindowStuff/SubInstances/terminalInstance/terminalInstance.h"
#include "../../WindowStuff/SubInstances/newTerminalInstance/newTerminalInstance.h"
#include <stdint.h>
#include "maabMem/maabMemHandler.h"

class TaskMAAB : public Task
{
private:
    uint8_t* mem;
    uint64_t memLen;

    MAAB_MEM::MbMemHandler* memHandler;

    uint64_t writeInputInto;



    Window* window;
    NewTerminalInstance* newTerm;
    TerminalInstance* dTerm;
    int cyclesPerCall = 1000;

    uint64_t instrPointer = 0;
    uint32_t defCol = Colors.white;

    uint64_t subLastPos[100];
    int64_t subDeepness = 0;
    int64_t maxSubDeepness = 90;
    bool programEnded = false;

    enum DatatypeNumber : uint8_t
    {
        INT = 0,
        UINT = 1,
        SHORT = 2,
        USHORT = 3,
        LONG = 4,
        ULONG = 5,
        CHAR = 6,
        BOOL = 7,
        FLOAT = 8,
        DOUBLE = 9
    };

    const uint8_t datatypeSizes[10] =
    { 4,4, 2,2, 8,8, 1,1, 4,8 };

    enum OpNumber : uint8_t
    {
        PLUS,
        MINUS,
        MULT,
        DIVIDE,
        MOD,
        EQUALS,
        NOT_EQUALS,
        GREATER,
        GREATER_EQUALS,
        LESS,
        LESS_EQUAL,
        BOOL_AND,
        BOOL_OR,
        BOOL_NOT,
        BIT_AND,
        BIT_OR,
        BIT_NOT,
        BITSHIFT_LEFT,
        BITSHIFT_RIGHT

    };

    void Cast(DatatypeNumber typeFrom, uint64_t addrFrom, DatatypeNumber typeTo, uint64_t addrTo);
    void PrintVal(DatatypeNumber numType, uint64_t numAddr);
    void ShowBytes(uint8_t amount, uint64_t addr);
    void Math(OpNumber opNum, DatatypeNumber typeNum, uint64_t addr1, uint64_t addr2, uint64_t addrRes);



public:

    const char* errMsg = "<NO ERRORS>";
    int errCode = 0;
    bool waitInput;
    bool gotInput;
    char memUserInput[500];
    uint64_t memUserInputLen = 0;

    TaskMAAB(uint32_t codeLen, uint8_t* code, Window* window, TerminalInstance* newTerm);
    void Do();
    void Free();

};

TaskMAAB* NewMAABTask(uint32_t codeLen, uint8_t* code, Window* window, TerminalInstance* newTerm);


