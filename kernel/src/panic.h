#pragma once

#include "BasicRenderer.h"
#include "Cols.h" 

static void Panic(const char* panicMessage, const char* var, bool lock)
{
    GlobalRenderer->Clear(Colors.red);
    GlobalRenderer->Println("KERNEL PANIC AAAAAAAAAAAAAAAAAAAAAAAAAAA", Colors.white);
    GlobalRenderer->Println();
    GlobalRenderer->Println();
    GlobalRenderer->Println(panicMessage, var, Colors.white);
  
    if (lock)
        while(true);
} 

static void Panic(const char* panicMessage, const char* var)
{
    Panic(panicMessage, var, true);
}

static void Panic(const char* panicMessage)
{
    Panic(panicMessage, NULL, true);
}

static void Panic(const char* panicMessage, bool lock)
{
    Panic(panicMessage, NULL, lock);
}




