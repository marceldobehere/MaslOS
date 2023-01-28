#include "terminalInstance.h"
#include "../../../OSDATA/MStack/MStackM.h"
#include "../newTerminalInstance/newTerminalInstance.h"

TerminalInstance::TerminalInstance(OSUser* user)
{
    this->currentUser = user;
    //this->window = window;
    mode = commandMode::none;
    instanceType = InstanceType::Terminal;
    userlen = 0;
    takeInput = false;
    for (int i = 0; i < 256; i++)
    {
        terminalInput[i] = 0;
        lastTerminalInput[i] = 0;
    }
    printUser = false;
    newTermInstance = _Malloc(sizeof(NewTerminalInstance));
    *((NewTerminalInstance*)newTermInstance) = NewTerminalInstance();
}
void TerminalInstance::SetWindow(Window* window)
{
    this->window = window;
    ((NewTerminalInstance*)newTermInstance)->SetWindow(window);
}

bool TerminalInstance::GetBusy()
{
    return tasks.getCount() != 0;
}

void TerminalInstance::Free()
{
    while (tasks.getCount() > 0)
    {
        Task* tsk = tasks.elementAt(0);
        tasks.removeFirst();
        FreeTask(tsk);
    }

    tasks.free();
    
    if (newTermInstance != NULL)
    {
        ((NewTerminalInstance*)newTermInstance)->Free();
        _Free(newTermInstance);
        newTermInstance = NULL;
    }    
}

void TerminalInstance::HandleEnter()
{
    AddToStack();
    if (userlen > 0)
    {
        if (userlen > 255)
            userlen = 255;
            
        terminalInput[userlen] = 0;
        
        
        ParseCommand(terminalInput, lastTerminalInput, &currentUser, window);
        //GlobalRenderer->Println("ADDR 1: {}", ConvertHexToString((uint64_t)this), Colors.yellow);
        //GlobalRenderer->Println("ADDR 2: {}", ConvertHexToString((uint64_t)terminalInput), Colors.yellow);
        //GlobalRenderer->Println((char*)terminalInput);
        //GlobalRenderer->Print("> ");
        //GlobalRenderer->Println(userData);
    }
    else
    {
        mode = commandMode::none;
    }
    printUser = true;
    RemoveFromStack();
}

void PrintUser(NewTerminalInstance* newTermInstance, OSUser* user)
{
    AddToStack();
    if (newTermInstance == NULL)
    {
        RemoveFromStack();
        return;
    }
    if (user== NULL)
    {
        RemoveFromStack();
        return;
    }

    newTermInstance->Print(user->userName, user->colData.userColor);
    newTermInstance->Print("> ");
    RemoveFromStack();
}

void TerminalInstance::KeyboardPrintStart()
{
    AddToStack();
    if (window == NULL)
    {
        RemoveFromStack();
        return;
    }

    if (!window->allowKeyboardDrawing)
    {
        RemoveFromStack();
        return;
    }

    if (window->instance->instanceType == InstanceType::Terminal)
    {
        ((NewTerminalInstance*)newTermInstance)->Println();
        TerminalInstance* instance = (TerminalInstance*)window->instance;
        PrintUser(((NewTerminalInstance*)newTermInstance), instance->currentUser);
    }
    RemoveFromStack();
}

void TerminalInstance::Cls()
{
    NewTerminalInstance* tempInst = (NewTerminalInstance*)newTermInstance;
    tempInst->Clear();
    tempInst->Println("Masl OS v0.36", Colors.green);
    tempInst->Println("-------------", Colors.green);
    tempInst->Println();
    tempInst->Render();
}

void TerminalInstance::PrintUserIfNeeded()
{
    AddToStack();
    if (!printUser)
        return;
    if (GetBusy())
        return;
    
    if (window != NULL)
    {
        if (window->allowKeyboardDrawing)
        {
            ((NewTerminalInstance*)newTermInstance)->Println();
            PrintUser((NewTerminalInstance*)newTermInstance, currentUser);
            ClearInput(this);
        }
    }
    printUser = false;
    RemoveFromStack();
}