#include "terminalInstance.h"
#include "../../../OSDATA/MStack/MStackM.h"

TerminalInstance::TerminalInstance(OSUser* user, Window* window)
{
    this->currentUser = user;
    this->window = window;
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
}

bool TerminalInstance::GetBusy()
{
    return tasks.getCount() != 0;
}

void TerminalInstance::Free()
{
    tasks.free();
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
            window->renderer->Println();
            PrintUser(window, currentUser);
            ClearInput(this);
        }
    }
    printUser = false;
    RemoveFromStack();
}