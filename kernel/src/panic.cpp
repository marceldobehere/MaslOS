#include "panic.h"
#include "BasicRenderer.h"
#include "Cols.h" 
#include "OSDATA/osdata.h"
#include "OSDATA/MStack/MStackM.h"

void Panic(const char* panicMessage, const char* var, bool lock)
{
    AddToStack("Panic", "panic.cpp");
    GlobalRenderer->Clear(Colors.red);
    GlobalRenderer->Println();
    GlobalRenderer->Println();
    GlobalRenderer->Println("KERNEL PANIC AAAAAAAAAAAAAAAAAAAAAAAAAAA", Colors.white);
    GlobalRenderer->Println();
    GlobalRenderer->Println(panicMessage, var, Colors.white);
    GlobalRenderer->Println();
    PrintMStackTrace(osData.stackArr, osData.stackPointer);
    GlobalRenderer->Println();
    GlobalRenderer->Println();

    if (osData.crashCount == 0)
    {
        osData.debugTerminalWindow->position.x = GlobalRenderer->framebuffer->Width - (osData.debugTerminalWindow->size.width + 2);
        osData.debugTerminalWindow->position.y = 23;
        osData.debugTerminalWindow->parentFrameBuffer = GlobalRenderer->framebuffer;
        osData.debugTerminalWindow->Render();
    }

    osData.crashCount++;
    if (lock)
        while(true);
    RemoveFromStack();
} 

void Panic(const char* panicMessage, const char* var)
{
    Panic(panicMessage, var, true);
}

void Panic(const char* panicMessage)
{
    Panic(panicMessage, NULL, true);
}

void Panic(const char* panicMessage, bool lock)
{
    Panic(panicMessage, NULL, lock);
}




