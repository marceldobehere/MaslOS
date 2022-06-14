#include "panic.h"
#include "BasicRenderer.h"
#include "Cols.h" 
#include "OSDATA/osdata.h"
#include "OSDATA/MStack/MStackM.h"

void Panic(const char* panicMessage, const char* var, bool lock)
{
    AddToMStack(MStack("Panic", "panic.cpp"));
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
    if (lock)
        while(true);
    RemoveLastMStack();
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




