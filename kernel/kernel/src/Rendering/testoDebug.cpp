
#include "BasicRenderer.h"
#include "../kernelStuff/stuff/cstr.h"
#include "Cols.h"
#include "../cmdParsing/cstrTools.h"
#include "../memory/heap.h"


int PrintLayer = 0;
bool PrintedSpace = false;

void PrintSpaces()
{
    for (int i = 0; i < PrintLayer; i++)
        GlobalRenderer->Print("  ");
    
    PrintedSpace = true;
}

void ScrollUp(int amt)
{
    Framebuffer* fb = GlobalRenderer->framebuffer;
    uint64_t w = fb->PixelsPerScanLine;
    int64_t w2 = w - 502;
    if (w2 < 0)
        w2 = 0;
    uint64_t h = fb->Height;
    uint32_t* arr = ((uint32_t*)fb->BaseAddress);
    for (int i = amt; i < h; i++)
        for (int j = 0; j < w2; j++)
            arr[(i - amt) * w + j] = arr[i * w + j];
    for (int i = 0; i < amt; i++)
        for (int j = 0; j < w2; j++)
            arr[(h - (1 + i)) * w + j] = Colors.black;
    GlobalRenderer->CursorPosition.y -= amt;
}

void PrintMsgColSL(const char* msg, const char* var, uint32_t col)
{
    if (!PrintedSpace)
        PrintSpaces();

    GlobalRenderer->Print(msg, var, col);
    while (GlobalRenderer->CursorPosition.y > GlobalRenderer->framebuffer->Height - 32)
        ScrollUp(16);
}

void Println()
{
    PrintMsgColSL("\n\r", "", GlobalRenderer->color);
    PrintedSpace = false;
}

void PrintMsgCol(const char* msg, const char* var, uint32_t col)
{
    PrintMsgColSL(msg, var, col);
    Println();
}

void PrintMsgStartLayer(const char* msg)
{
    PrintMsgColSL("<", "", GlobalRenderer->color);
    PrintMsgColSL(msg, "", GlobalRenderer->color);
    PrintMsgCol(">", "", GlobalRenderer->color);
    PrintLayer++;
}
void PrintMsgEndLayer(const char* msg)
{
    PrintLayer--;
    PrintMsgColSL("</", "", GlobalRenderer->color);
    PrintMsgColSL(msg, "", GlobalRenderer->color);
    PrintMsgCol(">", "", GlobalRenderer->color);
}


void PrintMsg(const char* msg)
{
    PrintMsgCol(msg, "", GlobalRenderer->color);
}

void PrintMsg(const char* msg, const char* var)
{
    PrintMsgCol(msg, var, GlobalRenderer->color);
}

void PrintMsgCol(const char* msg, uint32_t col)
{
    PrintMsgCol(msg, "", col);
}





void PrintMsgSL(const char* msg)
{
    PrintMsgColSL(msg, "", GlobalRenderer->color);
}

void PrintMsgSL(const char* msg, const char* var)
{
    PrintMsgColSL(msg, var, GlobalRenderer->color);
}

void PrintMsgColSL(const char* msg, uint32_t col)
{
    PrintMsgColSL(msg, "", col);
}

#include "../OSDATA/osdata.h"

void PrintDebugTerminal()
{
    osData.debugTerminalWindow->position.x = GlobalRenderer->framebuffer->Width - 500;
    osData.debugTerminalWindow->position.y = 23;
    osData.debugTerminalWindow->parentFrameBuffer = GlobalRenderer->framebuffer;
    osData.debugTerminalWindow->Render(osData.debugTerminalWindow->framebuffer, GlobalRenderer->framebuffer, osData.debugTerminalWindow->position, osData.debugTerminalWindow->size, osData.debugTerminalWindow);
}