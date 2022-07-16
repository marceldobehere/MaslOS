#pragma once

#include "../kernelStuff/IO/IO.h"
#include "../kernelStuff/stuff/math.h"
#include "../Rendering/BasicRenderer.h"
#include "../kernelStuff/stuff/cstr.h"
#include "../Rendering/Cols.h"
#include "../kernelStuff/kernelFiles/ImageStruct.h"
#include "../kernelStuff/kernelFiles/ZIPStruct.h"
#include "../customClasses/list.h"

extern int mouseCycleSkip;

extern kernelFiles::ZIPFile* mouseZIP;
extern kernelFiles::ImageFile* currentMouseImage;
extern const char* currentMouseImageName;
extern List<MousePacket> mousePackets;

void Mousewait();

void MousewaitInput();

void MouseWrite(uint8_t value);

uint8_t MouseRead();

void InitPS2Mouse(kernelFiles::ZIPFile* _mouseZIP, const char* _mouseName);

void MouseWrite(uint8_t value);

void HandlePS2Mouse(uint8_t data);

void ProcessMousePacket(MousePacket packet);
void ProcessMousePackets(int limit);
void ProcessMousePackets();



void DrawMousePointer();
void DrawMousePointer1(PointerFramebuffer* framebuffer);
void DrawMousePointer2(PointerFramebuffer* framebuffer, MPoint mousePos);

// void LoadFromBuffer(MPoint point, PointerFramebuffer* framebuffer);
// void SaveIntoBuffer(MPoint point, PointerFramebuffer* framebuffer);
// void DrawMouseBuffer(MPoint point, PointerFramebuffer* framebuffer);

extern uint32_t mouseColFront, mouseColBack;

extern MPoint MousePosition;