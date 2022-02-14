#pragma once

#include "BasicRenderer.h"
#include "Cols.h" 

static void Panic(const char* panicMessage, const char* var)
{
    GlobalRenderer->Clear(Colors.red);
    GlobalRenderer->Println("KERNEL PANIC AAAAAAAAAAAAAAAAAAAAAAAAAAAA", Colors.white);
    GlobalRenderer->Println();
    GlobalRenderer->Println();
    GlobalRenderer->Println(panicMessage, var, Colors.white);
  
    while(true);
} 

static void Panic(const char* panicMessage)
{
    Panic(panicMessage, NULL);
}

