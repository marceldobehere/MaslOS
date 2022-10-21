#pragma once

#pragma once
#include "../../../OSDATA/userdata.h"
#include "../defaultInstance/defaultInstance.h"
#include "../../Window/window.h"
#define LIST_INCLUDE_CONSOLE_CHAR
#include "../../../customClasses/list.h"
#undef LIST_INCLUDE_CONSOLE_CHAR

class NewTerminalInstance : public DefaultInstance
{
    private:
        Window* window;
        bool printUser;
        void WriteStringIntoList(const char* str, const char* var);
        void WriteStringIntoList(const char* str, const char* var, bool allowEscape);
        void WriteVarStringIntoList(const char* chrs, dispVar vars[]);
        List<ConsoleChar>* AddNewLine();

    public:
    // OSUser* currentUser;
    int32_t scrollX, scrollY, oldScrollX, oldScrollY, oldHeight, oldWidth;
    uint32_t backgroundColor;
    uint32_t foregroundColor;
    List<List<ConsoleChar>*> textData;
    ConsoleChar* tempPixels;
    ConsoleChar* tempPixels2;
    
    NewTerminalInstance(/*OSUser* user, */Window* window);
    
    void WriteText(const char* text);
    void Render();
    void RenderCharChanges();
    void Clear();
    void Free();

    void Println();
    void Print(const char* msg);
    void Print(const char* chrs, const char* var);
    void Print(char chr);
    void Print(const char* chrs, uint32_t col);
    void Print(const char* chrs, const char* var, uint32_t col);
    void Print(const char* chrs, dispVar vars[]);
    void Print(const char *chrs, dispVar vars[], uint32_t col);
    void DeleteLastCharInLine();

    void Println(const char* msg);
    void Println(const char* chrs, const char* var);
    void Println(const char* chrs, uint32_t col);
    void Println(const char* chrs, const char* var, uint32_t col);
    void Println(const char* chrs, dispVar vars[]);
    void Println(const char *chrs, dispVar vars[], uint32_t col);


    //List<Task*> tasks;
    //void HandleEnter();
    //void PrintUserIfNeeded();
    //char lastTerminalInput[256];
    //char terminalInput[256];
    //bool GetBusy();
    //uint64_t userlen;

};
