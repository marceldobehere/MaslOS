#pragma once

#include "../../Rendering/BasicRenderer.h"
#include "efiMemory.h"
#include "../../Rendering/Cols.h"
#include "../kernelFiles/FileStruct.h"  //kernelFiles::
#include "../kernelFiles/ZIPStruct.h"
#include "../other_IO/acpi/acpi.h"


struct BootInfo
{
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_font;
    kernelFiles::ImageFile* bgImage;
    kernelFiles::ImageFile* testImage;
	kernelFiles::ImageFile* bootImage;
	kernelFiles::ImageFile* MButton;
	kernelFiles::ImageFile* MButtonS;
    kernelFiles::ZIPFile* mouseZIP;
	kernelFiles::ZIPFile* windowButtonZIP;
	kernelFiles::ZIPFile* windowIconsZIP;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
    ACPI::RSDP2* rsdp;
};

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize);

void PrintEFIMemData(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize, BasicRenderer* temp );

void _memset(void* start, uint8_t value, uint64_t num);

void _memcpy(void* src, void* dest, uint64_t size);