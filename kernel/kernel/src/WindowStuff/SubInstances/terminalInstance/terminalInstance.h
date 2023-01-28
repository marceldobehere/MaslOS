#pragma once
#include "../../../OSDATA/userdata.h"
#include "../defaultInstance/defaultInstance.h"

#include "../../Window/window.h"
#define LIST_INCLUDE_TASK
#include "../../../customClasses/list.h"
#undef LIST_INCLUDE_TASK

enum commandMode
{
    none,
    enterPassword,
    enterText
};


class TerminalInstance : public DefaultInstance
{
    private:
        Window* window;
        bool printUser;
        

    public:
    List<Task*> tasks = List<Task*>(4);
    OSUser* currentUser;
    char lastTerminalInput[256];
    char terminalInput[256];
    bool GetBusy();
    bool takeInput;
    bool closeWindowAfterTask = false;
    uint64_t userlen;
    TerminalInstance(OSUser* user);
    void KeyboardPrintStart();
    void HandleEnter();
    void PrintUserIfNeeded();
    void Free();
    void Cls();
    void SetWindow(Window* window);
    commandMode mode;
    void* newTermInstance;

};