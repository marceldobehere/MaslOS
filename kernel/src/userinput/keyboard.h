#pragma once
#include <stdint.h>
#include "../OSDATA/userdata.h"
#include "../WindowStuff/Window/window.h"
#include "../WindowStuff/SubInstances/terminalInstance/terminalInstance.h"

void ClearInput(TerminalInstance* instance);

void HandleEnter();

void InitKeyboard();

//void KeyboardPrintStart(Window* window);

//void PrintUser(Window* window, OSUser* user);

void HandleKeyboard(uint8_t scancode);
