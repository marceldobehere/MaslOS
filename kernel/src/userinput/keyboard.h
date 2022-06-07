#pragma once
#include <stdint.h>
#include "../OSDATA/userdata.h"

void ClearInput();

void HandleEnter();

void InitKeyboard();

void KeyboardPrintStart();

void PrintUser(OSUser* user);

void HandleKeyboard(uint8_t scancode);
