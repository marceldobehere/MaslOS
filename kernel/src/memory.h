#pragma once

#include "BasicRenderer.h"
#include <stddef.h>
#include "cstr.h"
#include "efiMemory.h"
#include "Cols.h"
#include "kernelFiles/FileStruct.h"  //kernelFiles::
#include "kernelFiles/ZIPStruct.h"
#include "acpi/acpi.h"

struct BootInfo
{
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_font;
    kernelFiles::ImageFile* bgImage;
    kernelFiles::ImageFile* testImage;
    kernelFiles::ZIPFile* mouseZIP;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
    ACPI::RSDP2* rsdp;
};

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize);

void PrintEFIMemData(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize, BasicRenderer* temp );

void memset(void* start, uint8_t value, uint64_t num);