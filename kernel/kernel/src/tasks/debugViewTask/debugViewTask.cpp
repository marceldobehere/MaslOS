#include "debugViewTask.h"
#include "../../kernelStuff/other_IO/pit/pit.h"
#include "../../memory/heap.h"
#include "../../interrupts/interrupts.h"
#include "../../cStdLib/cstrTools.h"
#include "../../OSDATA/MStack/MStackM.h"
#include "../../OSDATA/osStats.h"


TaskDebugViewer::TaskDebugViewer(Window* window, char* coolBuf, uint64_t coolBufLen)
{
    this->window = window;
    this->coolBuf = coolBuf;
    this->coolBufLen = coolBufLen;
    waitForInput = false;
    lastL = 0;
    lastB = false;
    showAddr = true;
    done = false;
    currentAddr = (int64_t)RAM_START_ADDR;
    if (coolBuf != NULL)
        currentAddr = (uint64_t)0;
    scrollY = 0;
    type = TaskType::DEBUG_VIEWER;
    callCount = 0;

    oldTitle = window->title;
    exit = false;
    if (coolBuf == NULL)
        window->title = "Debug RAM Viewer";
    else
        window->title = "Debug File Viewer";
    osData.windowPointerThing->UpdateWindowBorder(window);
    nextTime = PIT::TimeSinceBootMS();

    ((TerminalInstance*)window->instance)->userlen = 0;
    ((TerminalInstance*)window->instance)->takeInput = false;
    oldIcon = window->icon;
    window->icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.DEBUG];
}

void TaskDebugViewer::Do()
{
    AddToStack();
    if (exit)
    {
        window->title = oldTitle;
        done = true;
        window->icon = oldIcon;
        window->renderer->Clear(Colors.black);
        osData.windowPointerThing->UpdateWindowBorder(window);
        ((NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance))->Reload();
        RemoveFromStack();
        return;
    }

    if (waitForInput)
    {
        ((TerminalInstance*)window->instance)->takeInput = true;
        if (KeyboardScancodeState[0x0E] && window==activeWindow)
        {
            if (!lastB)
            {
                lastB = true;
                if (((TerminalInstance*)window->instance)->userlen > 0)
                    ((TerminalInstance*)window->instance)->userlen--;
            }
        }
        else
            lastB = false;
        
        if ((((TerminalInstance*)window->instance)->userlen != lastL || KeyboardScancodeState[0x1C]) && window==activeWindow)
        {
            int l = ((TerminalInstance*)window->instance)->userlen;
            lastL = l;
            char* blehus = ((TerminalInstance*)window->instance)->terminalInput;
            char input = blehus[l - 1];
            blehus[l] = 0;
            window->renderer->Clear(Colors.black);
            window->renderer->Println("Enter address:");
            window->renderer->Println(blehus);

            if (l == 16 || (l != 0 && KeyboardScancodeState[0x1C]))
            {
                blehus[16] = 0;
                currentAddr = ConvertStringToLongHex(blehus);   
                scrollY = 0;

                ((TerminalInstance*)window->instance)->userlen = 0;
                ((TerminalInstance*)window->instance)->takeInput = false;
                waitForInput = false;
            }
            RemoveFromStack()
            return;
        }
        else
        {
            RemoveFromStack()
            return;
        }
    }


    uint64_t cTime = PIT::TimeSinceBootMS();


    if (cTime < nextTime )
    {
        RemoveFromStack();
        return;        
    }
    nextTime = cTime + 100;

    if (KeyboardScancodeState[0x11] && window==activeWindow)
    {
        int amt = 4;
        if (KeyboardScancodeState[0x2A] || KeyboardScancodeState[0x36])
            amt = 16;
        if (KeyboardScancodeState[0x1D])
            amt = 1;

        scrollY -= amt;
        if (scrollY < 0)
            scrollY = 0;
    }
    if (KeyboardScancodeState[0x1F] && window==activeWindow)
    {
        int amt = 4;
        if (KeyboardScancodeState[0x2A] || KeyboardScancodeState[0x36])
            amt = 16;
        if (KeyboardScancodeState[0x1D])
            amt = 1;

        scrollY += amt;
    }
    if (KeyboardScancodeState[0x12] && window==activeWindow)
        showAddr = true;
    if (KeyboardScancodeState[0x20] && window==activeWindow)
        showAddr = false;

    if (KeyboardScancodeState[0x10] && window==activeWindow)
    {
        waitForInput = true;
        window->renderer->Clear(Colors.black);
        window->renderer->Println("Enter address:");
        RemoveFromStack();
        return;
    }
    if (KeyboardScancodeState[0x2D] && window==activeWindow) 
    {
        exit = true;
        return;
    }




    window->renderer->Clear(Colors.black);

    int xCount = 8;
    int yCount = 10;

    yCount = (window->size.height / 16) - 6;
    if (yCount < 0)
        yCount = 0;

    int xCount2 = ((((window->size.width / 8) - (10 + (showAddr ? 10 : 0))) / 2) / 17);

    xCount = xCount2 * 8;

    if (xCount < 0)
        xCount = 0;

    uint64_t startAddr = (uint64_t)currentAddr + scrollY * xCount;

    window->renderer->Println("Debug RAM Viewer", Colors.yellow);
    window->renderer->Println("ADDR:   {}", ConvertHexToString((uint64_t)currentAddr), Colors.yellow);
    window->renderer->Println("OFFSET: {}", ConvertHexToString((uint64_t)scrollY * xCount), Colors.yellow);
    window->renderer->Println("ADDR 2: {}", ConvertHexToString(startAddr), Colors.yellow);
    window->renderer->Println("W/S - Scroll, X - Exit, E/D - Show/Hide Addr, Q - Enter HEX ADDR", Colors.yellow);
    window->renderer->Println("");

    // window->renderer->Print("0F 0F 0F 0F 0F 0F 0F 0F");
    // window->renderer->Print("   ");
    // window->renderer->Print("........");
    // window->renderer->Println();

    char* coolBufEnd = NULL;

    if (coolBuf != NULL)
        coolBufEnd = coolBuf + coolBufLen;


    for (int y = 0; y < yCount; y++)
    {
        uint8_t* tempArr = (uint8_t*)startAddr;
        if (coolBuf != NULL)
            tempArr = (uint64_t)startAddr + (uint8_t*)coolBuf;

        if (showAddr)
            window->renderer->Print("{} ", ConvertHexToString(startAddr), Colors.bgray);
        for (int x = 0; x < xCount; x++)
        {
            uint8_t val = 0;
            if (coolBufEnd == NULL || (char*)tempArr + x < coolBufEnd)
                val = tempArr[x];
            char testo[4];
            testo[2] = ' ';
            testo[3] = 0;
            if (val / 16 < 10)
                testo[0] = (val / 16) + '0';
            else
                testo[0] = (val / 16) + 'A' - 10;
            if (val % 16 < 10)
                testo[1] = (val % 16) + '0';
            else
                testo[1] = (val % 16) + 'A' - 10;

            window->renderer->Print(testo, Colors.bgreen);

            if (x % 8 == 7)
                window->renderer->Print(" ");
        }
        window->renderer->Print("   ");
        for (int x = 0; x < xCount; x++)
        {
            char chr = '?';
            if (coolBufEnd == NULL || (char*)tempArr + x < coolBufEnd)
                chr = tempArr[x];
            if (chr < 20 || chr > 126)
                chr = '.';
            char testo[2];
            testo[0] = chr;
            testo[1] = 0;
            window->renderer->Print(testo, Colors.white);

            if (x % 8 == 7)
                window->renderer->Print(" ");
        }
        window->renderer->Println();
        startAddr += xCount;

    }

    
    

    RemoveFromStack();
}

void TaskDebugViewer::Free()
{
    if (window->title != oldTitle)
    {
        window->title = oldTitle;
        done = true;
        window->icon = oldIcon;
        window->renderer->Clear(Colors.black);
        osData.windowPointerThing->UpdateWindowBorder(window);
        ((NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance))->Reload();
        RemoveFromStack();
        return;
    }
    if (coolBuf != NULL)
        _Free(coolBuf);
}

TaskDebugViewer* NewDebugViewerTask(Window* window, char* coolBuf, uint64_t coolBufLen)
{
    TaskDebugViewer* task = (TaskDebugViewer*)_Malloc(sizeof(TaskDebugViewer));
    *task = TaskDebugViewer(window, coolBuf, coolBufLen);
    return task;
}

TaskDebugViewer* NewDebugViewerTask(Window* window)
{
    TaskDebugViewer* task = (TaskDebugViewer*)_Malloc(sizeof(TaskDebugViewer));
    *task = TaskDebugViewer(window, NULL, 0);
    return task;
}