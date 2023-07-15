#pragma once
#include <stdint.h>
#include "kbScancodeTranslation.h"
#include "../osData/userData.h"
#include "../WindowStuff/Window/window.h"
#include "../WindowStuff/SubInstances/terminalInstance/terminalInstance.h"


void ClearInput(TerminalInstance* instance);

void HandleEnter();

void InitKeyboard();

//void KeyboardPrintStart(Window* window);

//void PrintUser(Window* window, OSUser* user);

void HandleKeyboard(uint8_t scancode);

uint8_t TranslateScancode2(uint8_t scan);

extern bool KeyboardScancodeState[256];

void HandleKeyboardList(int amt);

void AddScancodeToKeyboardList(uint8_t scancode);

int GetKeyboardCount();

void InitKeyboardListRam();