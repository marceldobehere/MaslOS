#include "panic.h"
#include "../Rendering/BasicRenderer.h"
#include "../Rendering/Cols.h" 
#include "../OSDATA/osdata.h"
#include "../OSDATA/MStack/MStackM.h"

void Panic(const char* panicMessage, const char* var, bool lock)
{
    AddToStack();
    GlobalRenderer->ClearDotted(Colors.red);
    GlobalRenderer->Println();
    GlobalRenderer->Println();
    GlobalRenderer->Println("KERNEL PANIC AAAAAAAAAAAAAAAAAAAAAAAAAAA", Colors.white);
    GlobalRenderer->Println();
    GlobalRenderer->Println(panicMessage, var, Colors.white);
    GlobalRenderer->Println();

    osData.crashCount++;
    if (osData.crashCount == 1)
    {
        osData.debugTerminalWindow->position.x = GlobalRenderer->framebuffer->Width - (osData.debugTerminalWindow->size.width + 2);
        osData.debugTerminalWindow->position.y = 23;
        osData.debugTerminalWindow->parentFrameBuffer = GlobalRenderer->framebuffer;
        osData.debugTerminalWindow->Render(osData.debugTerminalWindow->framebuffer, GlobalRenderer->framebuffer, osData.debugTerminalWindow->position, osData.debugTerminalWindow->size, osData.debugTerminalWindow);
    }
    else
    {
        GlobalRenderer->Println("(BTW the rendering of the debug terminal is causing issues so no debug terminal)");
        GlobalRenderer->Println();
    }

    PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer);
    GlobalRenderer->Println();
    GlobalRenderer->Println();


    
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




