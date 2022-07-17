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
        void WriteStringIntoList(const char* str);
        void WriteStringIntoList(const char* str, bool allowEscape);
        List<ConsoleChar>* AddNewLine();

    public:
    OSUser* currentUser;
    int32_t scrollX, scrollY;
    uint32_t backgroundColor;
    uint32_t foregroundColor;
    List<List<ConsoleChar>*> textData;
    
    NewTerminalInstance(OSUser* user, Window* window);
    
    void WriteText(const char* text);
    void Render();
    void Clear();
    void Free();

    //List<Task*> tasks;
    //void HandleEnter();
    //void PrintUserIfNeeded();
    //char lastTerminalInput[256];
    //char terminalInput[256];
    //bool GetBusy();
    //uint64_t userlen;

};