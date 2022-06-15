#pragma once
#include "../../../OSDATA/userdata.h"
#include "../defaultInstance/defaultInstance.h"
#include "../../../customClasses/list.h"

class TerminalInstance : public DefaultInstance
{
    private:
        Window* window;
        bool printUser;

    public:
    List<Task*> tasks;
    OSUser* currentUser;
    char lastTerminalInput[256];
    char terminalInput[256];
    bool GetBusy();
    uint64_t userlen;
    TerminalInstance(OSUser* user, Window* window);
    void HandleEnter();
    void PrintUserIfNeeded();

};