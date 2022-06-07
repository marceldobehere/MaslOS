#pragma once

#include  "../IO.h"
#include "../math.h"
#include "../BasicRenderer.h"
#include "../cstr.h"
#include "../Cols.h"
  

void Mousewait();

void MousewaitInput();

void MouseWrite(uint8_t value);

uint8_t MouseRead();

void InitPS2Mouse();

void MouseWrite(uint8_t value);

void HandlePS2Mouse(uint8_t data);

void ProcessMousePacket();


void DrawMousePointer();

void LoadFromBuffer(MPoint point);
void SaveIntoBuffer(MPoint point);
void DrawMouseBuffer(MPoint point);

extern uint32_t mouseColFront, mouseColBack;