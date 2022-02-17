#pragma once

#include  "../IO.h"
#include "../math.h"
#include "../BasicRenderer.h"
#include "../cstr.h"
  

#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

#define PS2LeftButton   0b00000001
#define PS2MiddleButton 0b00000010
#define PS2RightButton  0b00000100

void Mousewait();

void MousewaitInput();

void MouseWrite(uint8_t value);

uint8_t MouseRead();

void InitPS2Mouse();


void HandlePS2Mouse(uint8_t data);

void ProcessMousePacket();