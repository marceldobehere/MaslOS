#pragma once
#include <stdint.h>


void ClearInput();

void HandleEnter();

void InitKeyboard();

void KeyboardPrintStart();

void PrintUser();

void HandleKeyboard(uint8_t scancode);


extern uint32_t userCol;

extern const char* userName;