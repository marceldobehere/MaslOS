#include "keyboard.h"

#include <stdint.h>
#include "kbScancodeTranslation.h"
#include "../BasicRenderer.h"
#include "../cmdParsing/cmdParser.h"
#include "../Cols.h"
#include "../OSDATA/userdata.h"
#include "../OSDATA/osdata.h"
#include "../tasks/enterHandler/taskEnterHandler.h"


bool lshift = false;
bool rshift = false;
bool allowArrows = false;


void ClearInput(TerminalInstance* instance)
{
    AddToStack("ClearInput", "userinput/keyboard.cpp");
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
    Panic("Old Function used!");
    // AddToStack("HandleEnter", "userinput/keyboard.cpp"));
    // if (activeWindow->instance->instanceType == InstanceType::Terminal)
    // {
    //     TerminalInstance* instance = (TerminalInstance*)activeWindow->instance;
    //     if (instance->userlen > 0)
    //     {
    //         if (instance->userlen > 255)
    //             instance->userlen = 255;
                
    //         instance->terminalInput[instance->userlen] = 0;
    //         ParseCommand(instance->terminalInput, instance->lastTerminalInput, &instance->currentUser, activeWindow);
    //         //GlobalRenderer->Print("> ");
    //         //GlobalRenderer->Println(userData);
    //     }
    //     if (activeWindow != NULL)
    //     {
    //         if (activeWindow->allowKeyboardDrawing)
    //         {
    //             activeWindow->renderer->Println();
    //             PrintUser(activeWindow, instance->currentUser);
    //             ClearInput(instance);
    //         }
    //     }
    // }
    // RemoveFromStack();
}

void InitKeyboard()
{

}

void KeyboardPrintStart(Window* window)
{
    AddToStack("KeyboardPrintStart", "userinput/keyboard.cpp");
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
        window->renderer->CursorPosition.y += 16;
        TerminalInstance* instance = (TerminalInstance*)window->instance;
        PrintUser(window, instance->currentUser);
    }
    RemoveFromStack();
}

void PrintUser(Window* window, OSUser* user)
{
    AddToStack("PrintUser", "userinput/keyboard.cpp");
    if (window == NULL)
    {
        RemoveFromStack();
        return;
    }
    if (user== NULL)
    {
        RemoveFromStack();
        return;
    }

    window->renderer->Print(user->userName, user->colData.userColor);
    window->renderer->Print("> ");
    RemoveFromStack();
}

void HandleKeyboard(uint8_t scancode)
{
    AddToStack("HandleKeyboard", "userinput/keyboard.cpp");
    if (scancode == ARR_LEFT)
    {  
        int64_t index = osData.windows.getIndexOf(activeWindow);
        index = (index - 1 + osData.windows.getCount()) % osData.windows.getCount();
        activeWindow = osData.windows[index];
    }
    else if (scancode == ARR_RIGHT)
    {
        int64_t index = osData.windows.getIndexOf(activeWindow);
        index = (index + 1) % osData.windows.getCount();
        activeWindow = osData.windows[index];
    }
    else if (scancode == ARR_UP)
    {  
        // int64_t index = osData.windows.getIndexOf(activeWindow);
        // if (index == -1)
        // {
        //     RemoveFromStack();
        //     return;
        // }
        // osData.windows[index]->newSize.width += 10;
    }
    else if (scancode == ARR_DOWN)
    {
        // int64_t index = osData.windows.getIndexOf(activeWindow);
        // if (index == -1)
        // {
        //     RemoveFromStack();
        //     return;
        // }
        // if (osData.windows[index]->newSize.width >= 10)
        //     osData.windows[index]->newSize.width -= 10;
    }


    if (activeWindow == NULL)
    {
        RemoveFromStack();
        return;
    }


    if (scancode == LeftShift)
        lshift = true; 
    else if (scancode == LeftShift + 0x80)
        lshift = false;
    else if (scancode == RightShift)
        rshift = true; 
    else if (scancode == RightShift + 0x80)
        rshift = false;
    else if (scancode == Enter)
    {
        if (activeWindow->instance->instanceType == InstanceType::Terminal)
        {
            TerminalInstance* instance = (TerminalInstance*)activeWindow->instance;
            if (!instance->GetBusy())
            {
                if (activeWindow->allowKeyboardDrawing)
                    activeWindow->renderer->Println();
                instance->tasks.add(NewEnterTask(instance));
            }
        }
    }
    else if (scancode == Backspace)
    {
        if (activeWindow->instance->instanceType == InstanceType::Terminal)
        {
            TerminalInstance* instance = (TerminalInstance*)activeWindow->instance;
            if (instance->userlen > 0 && !instance->GetBusy())
            {
                instance->userlen--;
                instance->terminalInput[instance->userlen] = 0;
                if (activeWindow->allowKeyboardDrawing)
                    activeWindow->renderer->CursorPosition.x -= 8; 
                if (activeWindow->allowKeyboardDrawing)
                    activeWindow->renderer->delChar(activeWindow->renderer->CursorPosition.x, activeWindow->renderer->CursorPosition.y);
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

        if (ascii != 0)
        {
            if (activeWindow->instance->instanceType == InstanceType::Terminal)
            {
                TerminalInstance* instance = (TerminalInstance*)activeWindow->instance;

                if (activeWindow->allowKeyboardDrawing && !instance->GetBusy())
                {
                    if (instance->mode == commandMode::none)
                        activeWindow->renderer->Print(ascii);
                    else if (instance->mode == commandMode::enterText)
                        activeWindow->renderer->Print(ascii);
                    else if (instance->mode == commandMode::enterPassword)
                        activeWindow->renderer->Print("*");
                }

                if (instance->userlen < 255 && !instance->GetBusy())
                {
                    instance->terminalInput[instance->userlen] = ascii;
                    instance->userlen++;
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