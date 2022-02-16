#pragma once

#include <stdint.h>
#include "kbScancodeTranslation.h"
#include "../BasicRenderer.h"

bool lshift = false;
bool rshift = false;

void HandleKeyboard(uint8_t scancode)
{
    if (scancode == LeftShift)
        lshift = true; 
    else if (scancode == LeftShift + 0x80)
        lshift = false;
    else if (scancode == RightShift)
        rshift = true; 
    else if (scancode == RightShift + 0x80)
        rshift = false;
    else if (scancode == Enter)
        GlobalRenderer->Println();
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
    } 
    else if (scancode == ARR_UP)
    {
        if (GlobalRenderer->CursorPosition.y >= 16)
            GlobalRenderer->CursorPosition.y -= 16;
    }
    else if (scancode == ARR_DOWN) 
        GlobalRenderer->CursorPosition.y += 16;
    else if (scancode == ARR_LEFT)
    {  
        if (GlobalRenderer->CursorPosition.x >= 8)
            GlobalRenderer->CursorPosition.x -= 8; 
        else if (GlobalRenderer->CursorPosition.y >= 16)
        {
            GlobalRenderer->CursorPosition.y -= 16;
            GlobalRenderer->CursorPosition.x = GlobalRenderer->framebuffer->Width - 8;
        }
    }
    else if (scancode == ARR_RIGHT)
        GlobalRenderer->CursorPosition.x += 8; 
    else
    {
        char ascii = QWERTYKeyboard::Translate(scancode, lshift || rshift);

        if (ascii != 0)
            GlobalRenderer->Print(ascii);
    }
} 


