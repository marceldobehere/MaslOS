#pragma once

#include  "../IO.h"
#include "../math.h"
#include "../BasicRenderer.h"
#include "../cstr.h"
#include "../Cols.h"
#include "../kernelFiles/ImageStruct.h"
#include "../kernelFiles/ZIPStruct.h"

extern int mouseCycleSkip;

extern kernelFiles::ZIPFile* mouseZIP;
extern kernelFiles::ImageFile* currentMouseImage;
extern const char* currentMouseImageName;

void Mousewait();

void MousewaitInput();

void MouseWrite(uint8_t value);

uint8_t MouseRead();

void InitPS2Mouse(kernelFiles::ZIPFile* _mouseZIP, const char* _mouseName);

void MouseWrite(uint8_t value);

void HandlePS2Mouse(uint8_t data);

void ProcessMousePacket();


void DrawMousePointer();
void DrawMousePointer1(Framebuffer* framebuffer);
void DrawMousePointer2(Framebuffer* framebuffer);

void LoadFromBuffer(MPoint point, Framebuffer* framebuffer);
void SaveIntoBuffer(MPoint point, Framebuffer* framebuffer);
void DrawMouseBuffer(MPoint point, Framebuffer* framebuffer);

extern uint32_t mouseColFront, mouseColBack;

extern MPoint MousePosition;