#pragma once

#include "Rendering/Framebuffer.h"
#include "kernelStuff/other_IO/acpi/acpi.h"
#include "kernelStuff/kernelFiles/kernelFiles.h"
#include "kernelStuff/kernelFiles/ImageStruct.h"
#include "kernelStuff/kernelFiles/FileStruct.h"
#include "kernelStuff/kernelFiles/ZIPStruct.h"

#include "Rendering/S-Font.h"

struct MaslOsAssetStruct
{
    // Images
    kernelFiles::ImageFile* bgImage;
    kernelFiles::ImageFile* testImage;
	kernelFiles::ImageFile* bootImage;
	kernelFiles::ImageFile* MButton;
	kernelFiles::ImageFile* MButtonS;

    // Zips
    kernelFiles::ZIPFile* mouseZIP;
	kernelFiles::ZIPFile* windowButtonZIP;
	kernelFiles::ZIPFile* windowIconsZIP;

    kernelFiles::ZIPFile* maabZIP;
    kernelFiles::ZIPFile* otherZIP;

    // EFI Mem?

};


void bootTest(Framebuffer fb, ACPI::RSDP2* rsdp, PSF1_FONT* psf1_font, MaslOsAssetStruct* assets, void* freeMemStart, void* extraMemStart, uint64_t freeMemSize, void* kernelStart, uint64_t kernelSize, void* kernelStartV);

void RenderLoop();
void RecoverDed();