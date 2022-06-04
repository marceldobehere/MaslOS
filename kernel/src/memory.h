#pragma once

#include "BasicRenderer.h"
#include <stddef.h>
#include "cstr.h"
#include "efiMemory.h"
#include "Cols.h"

struct BootInfo
{
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
};

static uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize)
{
    uint64_t size = 0;
    for (int i = 0; i < mMapEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        size += desc->numPages * 4096;
    }

    return size;
}


static void PrintEFIMemData(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize, BasicRenderer* temp )
{
    {
        temp->Println("EFI Memory Data:");

        

        temp->Print("There are "); 
        temp->Print(to_string(mMapEntries)); 
        temp->Println(" Entries: "); 


        for (int i = 0; i < mMapEntries; i++)
        {
            EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
            temp->Print(EFI_MEMORY_TYPE_STRINGS[desc->type]);
            temp->Print(" - "); 
            temp->color = Colors.cyan;
            temp->Print(to_string(desc->numPages * 4096 / 1024)); 
            temp->Print(" KB"); 
            temp->color = Colors.white;
            temp->Println("."); 
        }


    }
}


static void memset(void* start, uint8_t value, uint64_t num)
{
    for (uint64_t i = 0; i < num; i++)
        *(uint8_t*)((uint64_t)start + i) = value;
}