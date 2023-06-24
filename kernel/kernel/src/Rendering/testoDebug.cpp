
#include "BasicRenderer.h"
#include "../kernelStuff/stuff/cstr.h"
#include "Cols.h"
#include "../cmdParsing/cstrTools.h"
#include "../memory/heap.h"
#include "../OSDATA/osdata.h"

bool PrintAll = true;
int PrintLayer = 0;
bool PrintedSpace = false;

int menuBarPosX = 0;
int menuBarPosY = 0;

int menuBarSizeY = 40; // can vary
int menuBarWidthScale = 4;
int menuborderSize = 4;

void PrepBootScreen()
{
    if (osData.verboseBoot)
        return;

    int menuBarSizeX = 100 * menuBarWidthScale;
    //menuBarSizeY *= menuBarWidthScale;
    //menuborderSize *= menuBarWidthScale;

    menuBarPosX = GlobalRenderer->framebuffer->Width / 2 - menuBarSizeX / 2;
    menuBarPosY = GlobalRenderer->framebuffer->Height / 2 - menuBarSizeY / 2;

    GlobalRenderer->Clear(Colors.black);

    GlobalRenderer->CursorPosition.y = menuBarPosY - 30;
    GlobalRenderer->CursorPosition.x = GlobalRenderer->framebuffer->Width / 2 - 
    12 * 8 / 2;
    GlobalRenderer->Println("MASL OS BOOT", Colors.yellow);

    GlobalRenderer->Clear(
        menuBarPosX - menuborderSize, menuBarPosY - menuborderSize, 
        menuBarPosX + menuBarSizeX + menuborderSize,
        menuBarPosY + menuBarSizeY + menuborderSize,
    Colors.white);

    GlobalRenderer->Clear(
        menuBarPosX, menuBarPosY, 
        menuBarPosX + menuBarSizeX,
        menuBarPosY + menuBarSizeY,
    Colors.dgray);

    //while (true);
}

void PercentDone(int percent)
{
    if (osData.verboseBoot)
        return;
    if (!osData.booting)
    {
        osData.booting = true;
        osData.verboseBoot = true;
        GlobalRenderer->Clear(Colors.black);
        return;
    }

    for (int x = 0; x < 10; x++)
    for (int i = 0; i < percent; i++)
    {
        GlobalRenderer->Clear(
            menuBarPosX + i * menuBarWidthScale, 
            menuBarPosY, 
            menuBarPosX + (i + 1) * menuBarWidthScale,
            menuBarPosY + menuBarSizeY,
        ((((i + 70) * 140) / 100) * 0x00010101) & 0xFF00FF00 + 0xFF000000);
    }
}
int maxSteps = 27;
int currStep = 0;
void StepDone()
{
    StepDone(currStep + 1);
}

void StepDone(int step)
{
    if (osData.verboseBoot)
        return;

    currStep = step;

    if (step > maxSteps)
        Panic("TOO MANY STEPS", true);
    PercentDone((step * 100) / maxSteps);
}


void PrintSpaces()
{
    GlobalRenderer->CursorPosition.x += PrintLayer * 8;
    // for (int i = 0; i < PrintLayer; i++)
    //     GlobalRenderer->Print("  ");
    
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
    if (!PrintAll || !osData.verboseBoot)
        return;
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
    if (!PrintAll || !osData.verboseBoot)
        return;

    osData.debugTerminalWindow->position.x = GlobalRenderer->framebuffer->Width - 500;
    osData.debugTerminalWindow->position.y = 23;
    osData.debugTerminalWindow->parentFrameBuffer = GlobalRenderer->framebuffer;
    osData.debugTerminalWindow->Render(osData.debugTerminalWindow->framebuffer, GlobalRenderer->framebuffer, osData.debugTerminalWindow->position, osData.debugTerminalWindow->size, osData.debugTerminalWindow);
}