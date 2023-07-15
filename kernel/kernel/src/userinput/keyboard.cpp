#include "keyboard.h"

#include <stdint.h>

#include "../rendering/BasicRenderer.h"
#include "../cmdParsing/cmdParser.h"
#include "../rendering/Cols.h"
#include "../osData/userData.h"
#include "../osData/osData.h"
#include "../tasks/enterHandler/taskEnterHandler.h"
#include "../tasks/taskMgrTask/taskMgrTask.h"
#include "../WindowStuff/SubInstances/connect4Instance/connect4Instance.h"
#include "../tasks/maab/maabTask.h"
#include "../WindowStuff/SubInstances/guiInstance/guiInstance.h"
#include "../tasks/closeWindow/taskWindowClose.h"





bool lshift = false;
bool rshift = false;
bool lAlt = false;
bool allowArrows = false;


void ClearInput(TerminalInstance* instance)
{
    AddToStack();
    for (int i = 0; i < 256; i++)
    {
        instance->lastTerminalInput[i] = instance->terminalInput[i];
        instance->terminalInput[i] = 0;
    }
    instance->userlen = 0;
    RemoveFromStack();
}


void HandleEnter()
{
    Panic("Old Function used!", true);
}

bool KeyboardScancodeState[256];

#include "../cStdLib/list.h"

List<uint8_t>* scancodesToGo;
bool keyListInit = false;

void InitKeyboardListRam()
{
    scancodesToGo = new List<uint8_t>(10);
    scancodesToGo->clear();
    keyListInit = true;
}

void InitKeyboard()
{
    
    // outb(0x60, 0xF0);
    // io_wait();
    // outb(0x60, 0x01);
    // io_wait();
    lshift = false;
    rshift = false;
    lAlt = false;

    for (int i = 0; i < 256; i++)
        KeyboardScancodeState[i] = false;
}

uint8_t TranslateScancode2(uint8_t scan)
{
    return QWERTYKeyboard::Scancode2ToScancode1[scan];
}

int scrollSpeed = 8;



void HandleKeyboard(uint8_t scancode)
{
    if (scancode == LeftShift)
    {
        KeyboardScancodeState[scancode & ~0x80] = true;
        lshift = true; 
        return;
    }
    else if (scancode == LeftShift + 0x80)
    {
        KeyboardScancodeState[scancode & ~0x80] = false;
        lshift = false;
        return;
    }
    else if (scancode == RightShift)
    {
        KeyboardScancodeState[scancode & ~0x80] = true;
        rshift = true; 
        return;
    }
    else if (scancode == RightShift + 0x80)
    {
        KeyboardScancodeState[scancode & ~0x80] = false;
        rshift = false;
        return;
    }
    else if (scancode == LeftAlt)
    {
        KeyboardScancodeState[scancode & ~0x80] = true;
        lAlt = true; 
        return;
    }
    else if (scancode == LeftAlt + 0x80)
    {
        KeyboardScancodeState[scancode & ~0x80] = false;
        lAlt = false;
        return;
    }

    if (scancode & 0x80)
    {
        KeyboardScancodeState[scancode & ~0x80] = false;
        return;
    }
    else
    {
        if (KeyboardScancodeState[scancode] && 
            scancode != Backspace && 
            scancode != ARR_DOWN &&
            scancode != ARR_LEFT &&
            scancode != ARR_UP &&
            scancode != ARR_RIGHT)
        {
            return;
        }
        else
            KeyboardScancodeState[scancode] = true;
    }

    if (lAlt && scancode == 0x3B && activeWindow != NULL &&  // alt + F1 to close task
        activeWindow->instance != NULL && activeWindow->instance->instanceType == InstanceType::Terminal &&
        ((TerminalInstance*)activeWindow->instance)->newTermInstance != NULL)
    {
        TerminalInstance* bruh1 = ((TerminalInstance*)activeWindow->instance);
        NewTerminalInstance* bruh2 = ((NewTerminalInstance*)bruh1->newTermInstance);
        if (bruh1->tasks.getCount() > 0)
        {
            Task* tsk = bruh1->tasks.elementAt(0);
            bruh1->tasks.removeFirst();
            FreeTask(tsk);

            bruh2->Println("\n\nCurrent Task forcibly stopped!", Colors.bred);

            bruh1->PrintUserIfNeeded();
            return;
        }
    }

    if (lAlt && scancode == 0x3E && activeWindow != NULL)
    {
        osData.osTasks.add(NewWindowCloseTask(activeWindow));
        return;
    }

    if (lAlt && scancode == 0x40)
    {
        int64_t count = Taskbar::taskWindowList->getCount();
        int64_t index = Taskbar::taskWindowList->getIndexOf(activeWindow);
        index = (index + count + 1) % count;
        Window* w = Taskbar::taskWindowList->elementAt(index);
        osData.windowsToGetActive.add(w);

        return;
    }
    if (lAlt && scancode == 0x41)
    {
        //Window* oldActive = activeWindow;
        Window* mainWindow = (Window*)_Malloc(sizeof(Window), "Main Window");
        TerminalInstance* terminal = (TerminalInstance*)_Malloc(sizeof(TerminalInstance), "Terminal Instance");
        *terminal = TerminalInstance(&guestUser);
        *(mainWindow) = Window((DefaultInstance*)terminal, Size(600, 500), Position(10, 40), "Terminal Window", true, true, true);
        osData.windows.add(mainWindow);
        terminal->SetWindow(mainWindow);
        ((TerminalInstance*)mainWindow->instance)->Cls();
        //KeyboardPrintStart(mainWindow);
        ((TerminalInstance*)mainWindow->instance)->KeyboardPrintStart();

        osData.windowsToGetActive.add(mainWindow);
    }


    AddToStack();
    if (activeWindow != NULL &&
        activeWindow->instance != NULL &&
        activeWindow->instance->instanceType == InstanceType::GUI &&
        ((GuiInstance*)activeWindow->instance)->screen != NULL)
    {
        ((GuiInstance*)activeWindow->instance)->screen->KeyHit(GuiComponentStuff::KeyHitEventInfo(scancode, QWERTYKeyboard::Translate(scancode, lshift || rshift)));
        RemoveFromStack();
        return;
    }

    if (scancode == ARR_LEFT)
    {  
        if (lshift || rshift)
        {
            if (activeWindow != NULL)
                if (activeWindow->instance != NULL)
                    if (activeWindow->instance->instanceType == InstanceType::Terminal)
                        if (((TerminalInstance*)activeWindow->instance)->newTermInstance != NULL)
                        {
                            ((NewTerminalInstance*)((TerminalInstance*)activeWindow->instance)->newTermInstance)->scrollX -= scrollSpeed;
                            RemoveFromStack();
                            return;
                        }
        }
        // else if (lAlt)
        // {
        //     if (activeWindow != NULL)
        //         activeWindow->position.x--;
        // }
        else if (lAlt)
        {
            if (activeWindow != NULL)
            {
                activeWindow->newPosition.x -= 5;
                RemoveFromStack();
                return;
            }
            // int64_t count = Taskbar::taskWindowList->getCount();
            // int64_t index = Taskbar::taskWindowList->getIndexOf(activeWindow);
            // index = (index + count - 1) % count;
            // activeWindow = Taskbar::taskWindowList->elementAt(index);
            // activeWindow->moveToFront = true;
        }
    }
    else if (scancode == ARR_RIGHT)
    {
        if (lshift || rshift)
        {
            if (activeWindow != NULL)
                if (activeWindow->instance != NULL)
                    if (activeWindow->instance->instanceType == InstanceType::Terminal)
                        if (((TerminalInstance*)activeWindow->instance)->newTermInstance != NULL)
                        {
                            ((NewTerminalInstance*)((TerminalInstance*)activeWindow->instance)->newTermInstance)->scrollX += scrollSpeed;
                            RemoveFromStack();
                            return;
                        }
        }
        else if (lAlt)
        {
            if (activeWindow != NULL)
            {
                activeWindow->newPosition.x += 5;
                RemoveFromStack();
                return;
            }

            // int64_t count = Taskbar::taskWindowList->getCount();
            // int64_t index = Taskbar::taskWindowList->getIndexOf(activeWindow);
            // index = (index + count + 1) % count;
            // activeWindow = Taskbar::taskWindowList->elementAt(index);
            // activeWindow->moveToFront = true;
        }
    }
    else if (scancode == ARR_UP)
    {  
        if (lshift || rshift)
        {
            if (activeWindow != NULL)
                if (activeWindow->instance != NULL)
                    if (activeWindow->instance->instanceType == InstanceType::Terminal)
                        if (((TerminalInstance*)activeWindow->instance)->newTermInstance != NULL)
                        {
                            ((NewTerminalInstance*)((TerminalInstance*)activeWindow->instance)->newTermInstance)->scrollY -= 2*scrollSpeed;
                            RemoveFromStack();
                            return;
                        }
        }
        else if (lAlt)
        {
            if (activeWindow != NULL)
            {
                activeWindow->newPosition.y -= 5;
                RemoveFromStack();
                return;
            }
        }
    }
    else if (scancode == ARR_DOWN)
    {
        if (lshift || rshift)
        {
            if (activeWindow != NULL)
                if (activeWindow->instance != NULL)
                    if (activeWindow->instance->instanceType == InstanceType::Terminal)
                        if (((TerminalInstance*)activeWindow->instance)->newTermInstance != NULL)
                        {
                            ((NewTerminalInstance*)((TerminalInstance*)activeWindow->instance)->newTermInstance)->scrollY += 2*scrollSpeed;
                            RemoveFromStack();
                            return;
                        }
        }
        else if (lAlt)
        {
            if (activeWindow != NULL)
            {
                activeWindow->newPosition.y += 5;
                RemoveFromStack();
                return;
            }
            // Window* oldActive = activeWindow;
            // Window* mainWindow = (Window*)_Malloc(sizeof(Window), "Main Window");
            // TerminalInstance* terminal = (TerminalInstance*)_Malloc(sizeof(TerminalInstance), "Terminal Instance");
            // *terminal = TerminalInstance(&guestUser);
            // *(mainWindow) = Window((DefaultInstance*)terminal, Size(600, 500), Position(10, 40), "Terminal Window", true, true, true);
            // osData.windows.add(mainWindow);
            // terminal->SetWindow(mainWindow);
            // ((TerminalInstance*)mainWindow->instance)->Cls();
            // //KeyboardPrintStart(mainWindow);
            // ((TerminalInstance*)mainWindow->instance)->KeyboardPrintStart();

            // activeWindow = mainWindow;          
            // mainWindow->moveToFront = true;
            // osData.mainTerminalWindow = mainWindow;

            // if (oldActive != NULL)
            // {
            //     osData.windowPointerThing->UpdateWindowBorder(oldActive);
            // }

            // RemoveFromStack();
            // return;
        }
    }


    if (activeWindow == NULL)
    {
        RemoveFromStack();
        return;
    }


    if (scancode == Enter)
    {
        if (activeWindow->instance == NULL)
        {
            RemoveFromStack();
            return;
        }

        if (activeWindow->instance->instanceType == InstanceType::Terminal)
        {
            TerminalInstance* instance = (TerminalInstance*)activeWindow->instance;
            TaskMAAB* maab = NULL;
            for (int i = 0; i < instance->tasks.getCount(); i++)
            {
                if (instance->tasks[i]->GetType() == TaskType::MAAB)
                {
                    maab = (TaskMAAB*)instance->tasks[i];
                    break;
                }
            }

            if (maab == NULL)
            {
                if (!instance->GetBusy())
                {
                    if (activeWindow->allowKeyboardDrawing)
                        ((NewTerminalInstance*)instance->newTermInstance)->Println();
                    instance->tasks.add(NewEnterTask(instance));
                }
            }
            else
            {
                if (maab->waitInput)
                {
                    maab->gotInput = true;
                }
            }
        }
        else if (activeWindow->instance->instanceType == InstanceType::Connect4)
        {
            Connect4Instance* instance = (Connect4Instance*)activeWindow->instance;
            if (instance->currentMode == Connect4Instance::CurrentMode::PLAYER_1_ENTER || instance->currentMode == Connect4Instance::CurrentMode::PLAYER_2_ENTER
                || instance->currentMode == Connect4Instance::CurrentMode::END || instance->currentMode == Connect4Instance::CurrentMode::START)
            {
                instance->HandleInput();
            }
        }
    }
    else if (scancode == Backspace)
    {
        if (activeWindow->instance == NULL)
        {
            RemoveFromStack();
            return;
        }

        if (activeWindow->instance->instanceType == InstanceType::Terminal)
        {
            TerminalInstance* instance = (TerminalInstance*)activeWindow->instance;

            TaskMAAB* maab = NULL;
            for (int i = 0; i < instance->tasks.getCount(); i++)
            {
                if (instance->tasks[i]->GetType() == TaskType::MAAB)
                {
                    maab = (TaskMAAB*)instance->tasks[i];
                    break;
                }
            }

            if (maab == NULL)
            {
                if (instance->userlen > 0 && !instance->GetBusy())
                {
                    instance->userlen--;
                    instance->terminalInput[instance->userlen] = 0;
                    if (activeWindow->allowKeyboardDrawing)
                        activeWindow->renderer->CursorPosition.x -= 8; 
                    if (activeWindow->allowKeyboardDrawing)
                        ((NewTerminalInstance*)instance->newTermInstance)->DeleteLastCharInLine();//activeWindow->renderer->delChar(activeWindow->renderer->CursorPosition.x, activeWindow->renderer->CursorPosition.y);
                }
            }
            else
            {
                if (maab->memUserInputLen > 0)
                {
                    maab->memUserInput[--maab->memUserInputLen] = 0;
                    ((NewTerminalInstance*)instance->newTermInstance)->DeleteLastCharInLine();
                }
            }
        }
        else if (activeWindow->instance->instanceType == InstanceType::Connect4)
        {
            Connect4Instance* instance = (Connect4Instance*)activeWindow->instance;
            if (instance->inputLen > 0 &&  (instance->currentMode == Connect4Instance::CurrentMode::PLAYER_1_ENTER || instance->currentMode == Connect4Instance::CurrentMode::PLAYER_2_ENTER))
            {
                instance->inputLen--;
                instance->userInput[instance->inputLen] = 0;
                activeWindow->renderer->CursorPosition.x -= 8; 
                activeWindow->renderer->delChar(activeWindow->renderer->CursorPosition.x, activeWindow->renderer->CursorPosition.y, instance->bgCol);
            }
        }
    } 
    else if (scancode == ARR_UP)
    {
        if (allowArrows && activeWindow->allowKeyboardDrawing)
        {
            if (activeWindow->renderer->CursorPosition.y >= 16)
                activeWindow->renderer->CursorPosition.y -= 16;
        }
    }
    else if (scancode == ARR_DOWN) 
    {
        if (allowArrows && activeWindow->allowKeyboardDrawing)
        {
            activeWindow->renderer->CursorPosition.y += 16;
        }
    }
    else if (scancode == ARR_LEFT)
    {  
        if (allowArrows && activeWindow->allowKeyboardDrawing)
        {
            if (activeWindow->renderer->CursorPosition.x >= 8)
                activeWindow->renderer->CursorPosition.x -= 8; 
            else if (activeWindow->renderer->CursorPosition.y >= 16)
            {
                activeWindow->renderer->CursorPosition.y -= 16;
                activeWindow->renderer->CursorPosition.x = GlobalRenderer->framebuffer->Width - 8;
            }
        }
    }
    else if (scancode == ARR_RIGHT)
    {
        if (allowArrows && activeWindow->allowKeyboardDrawing)
        {
            activeWindow->renderer->CursorPosition.x += 8; 
        }
    }
    else
    {
        char ascii = QWERTYKeyboard::Translate(scancode, lshift || rshift);

        if (activeWindow->instance == NULL)
        {
            RemoveFromStack();
            return;
        }

        if (ascii != 0)
        {
            if (activeWindow->instance->instanceType == InstanceType::Terminal)
            {
                TerminalInstance* instance = (TerminalInstance*)activeWindow->instance;

                //NewTerminalInstance* inst = NULL;
                TaskMAAB* maab = NULL;
                TaskTaskManager* taskMgr = NULL;
                for (int i = 0; i < instance->tasks.getCount(); i++)
                {
                    if (instance->tasks[i]->GetType() == TaskType::MAAB)
                    {
                        maab = (TaskMAAB*)instance->tasks[i];
                        break;
                    }
                    if (instance->tasks[i]->GetType() == TaskType::TASK_MGR)
                    {
                        taskMgr = (TaskTaskManager*)instance->tasks[i];
                        break;
                    }
                }
                if (maab != NULL)
                {
                    if (maab->waitInput && !maab->gotInput)
                    {
                        if (ascii == '\n' || maab->memUserInputLen >= 490)
                        {
                            maab->gotInput = true;
                        }
                        else if (ascii == '\b')
                        {
                            if (maab->memUserInputLen > 0)
                            {
                                maab->memUserInput[--maab->memUserInputLen] = 0;
                                ((NewTerminalInstance*)instance->newTermInstance)->DeleteLastCharInLine();
                            }
                        }
                        else
                        {
                            maab->memUserInput[maab->memUserInputLen++] = ascii;
                            ((NewTerminalInstance*)instance->newTermInstance)->Print(ascii);
                        }
                    }
                }
                else if (taskMgr != NULL)
                {
                    if (ascii == 'Q' || ascii == 'q')
                        taskMgr->exit = true;
                }
                else
                {
                    if ((activeWindow->allowKeyboardDrawing && !instance->GetBusy()))
                    {
                        if (instance->mode == commandMode::mode_none)
                            ((NewTerminalInstance*)instance->newTermInstance)->Print(ascii);
                        else if (instance->mode == commandMode::enterText)
                            ((NewTerminalInstance*)instance->newTermInstance)->Print(ascii);
                        else if (instance->mode == commandMode::enterPassword)
                            ((NewTerminalInstance*)instance->newTermInstance)->Print("*");
                    }

                    if ((instance->userlen < 255 && !instance->GetBusy()) || instance->takeInput)
                    {
                        instance->terminalInput[instance->userlen] = ascii;
                        instance->userlen++;
                    }
                }
            }
            else if (activeWindow->instance->instanceType == InstanceType::Connect4)
            {
                Connect4Instance* instance = (Connect4Instance*)activeWindow->instance;

                if (instance->currentMode != Connect4Instance::CurrentMode::NONE && instance->currentMode != Connect4Instance::CurrentMode::WAIT)
                {
                    if (instance->currentMode == Connect4Instance::CurrentMode::PLAYER_1_ENTER || 
                        instance->currentMode == Connect4Instance::CurrentMode::PLAYER_2_ENTER)
                        if (instance->inputLen < 4)
                            activeWindow->renderer->Print(ascii);

                    if (instance->inputLen < 4)
                    {
                        instance->userInput[instance->inputLen] = ascii;
                        instance->inputLen++;
                    }   
                }
            }
        }
    }

    RemoveFromStack();
} 



/*
    else if (scancode == Backspace)
    {
        if (GlobalRenderer->CursorPosition.x >= 8)
            GlobalRenderer->CursorPosition.x -= 8; 
        else if (GlobalRenderer->CursorPosition.y >= 16)
        {
            GlobalRenderer->CursorPosition.y -= 16;
            GlobalRenderer->CursorPosition.x = GlobalRenderer->framebuffer->Width - 8;
        }
        GlobalRenderer->delChar(GlobalRenderer->CursorPosition.x, GlobalRenderer->CursorPosition.y);
        if (userLen > 0)
        {
            userLen--;
            userData[userLen] = 0;
        }
    } 
*/






void HandleKeyboardList(int amt)
{
    if (!keyListInit)
        return;

    AddToStack();
    for (int i = 0; i < amt; i++)
    {
        if (scancodesToGo->getCount() < 1)
        {
            RemoveFromStack();
            return;
        }
        
        uint8_t current = scancodesToGo->elementAt(0);
        scancodesToGo->removeFirst();
        HandleKeyboard(current);
    }
    RemoveFromStack();
}

void AddScancodeToKeyboardList(uint8_t scancode)
{
    if (!keyListInit)
        return;

    scancodesToGo->add(scancode);
}

int GetKeyboardCount()
{
    if (!keyListInit)
        return 0;

    return scancodesToGo->getCount();
}

