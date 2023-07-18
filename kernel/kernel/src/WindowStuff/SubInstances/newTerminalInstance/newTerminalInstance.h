#pragma once

#pragma once
#include "../../../osData/userData.h"
#include "../defaultInstance/defaultInstance.h"
#include "../../Window/window.h"

#include "../../../cStdLib/list/list_consoleChar.h"

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
    bool redirectToSerial;
    int32_t scrollX, scrollY, oldScrollX, oldScrollY, oldHeight, oldWidth;
    uint32_t backgroundColor;
    uint32_t foregroundColor;
    List<List<ConsoleChar>*> textData;
    ConsoleChar* tempPixels;
    ConsoleChar* tempPixels2;
    
    NewTerminalInstance();
    
    void WriteText(const char* text);
    bool RenderCalled;
    void Render();
    void Reload();
    void DoRender();
    void RenderCharChanges();
    void Clear();
    void Free();
    void SetWindow(Window* window);

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
