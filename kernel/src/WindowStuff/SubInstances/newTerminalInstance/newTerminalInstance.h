#pragma once

#pragma once
#include "../../../OSDATA/userdata.h"
#include "../defaultInstance/defaultInstance.h"
#include "../../../customClasses/list.h"


class NewTerminalInstance : public DefaultInstance
{
    private:
        Window* window;
        bool printUser;

    public:
    OSUser* currentUser;
    int32_t scrollX, scrollY;
    
    NewTerminalInstance(OSUser* user, Window* window);
    
    void WriteText(const char* text);
    void Render();
    void Clear(uint32_t col);


    //List<Task*> tasks;
    //void HandleEnter();
    //void PrintUserIfNeeded();
    //char lastTerminalInput[256];
    //char terminalInput[256];
    //bool GetBusy();
    //uint64_t userlen;

};