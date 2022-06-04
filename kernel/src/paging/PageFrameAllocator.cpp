#include "PageFrameAllocator.h"
#include <stddef.h>
#include "../efiMemory.h"
#include "../bitmap.h"
#include "../memory.h"

PageFrameAllocator* GlobalAllocator;

void PageFrameAllocator::InitBitmap(size_t bitmapSize, void* bufferAddress)
{
    PageBitMap.Size = bitmapSize;
    PageBitMap.Buffer = (uint8_t*) bufferAddress;
    for (int i = 0; i < bitmapSize; i++)
        *(uint8_t*)(PageFrameAllocator::PageBitMap.Buffer + i) = 0;
}

void PageFrameAllocator::ReservePage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitMap[index])
        return;
    PageBitMap.Set(index, true);
    freeMemory -= 4096;
    reservedMemory += 4096;
}

void PageFrameAllocator::UnreservePage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (!PageBitMap[index])
        return;
    if(!PageBitMap.Set(index, false))
        return;
    if (pageBitmapIndex > index)
        pageBitmapIndex = index;
    freeMemory += 4096;
    reservedMemory -= 4096;
}

void PageFrameAllocator::ReservePages(void* address, uint64_t pageCount)
{
    for (int i = 0; i < pageCount; i++)
        ReservePage((void*)((uint64_t)address + (i * 4096)));
}

void PageFrameAllocator::UnreservePages(void* address, uint64_t pageCount)
{
    for (int i = 0; i < pageCount; i++)
        UnreservePage((void*)((uint64_t)address + (i * 4096)));
}


uint64_t PageFrameAllocator::GetFreeRAM()
{
    return freeMemory;
}
uint64_t PageFrameAllocator::GetUsedRAM()
{
    return usedMemory;
}
uint64_t PageFrameAllocator::GetReservedRAM()
{
    return reservedMemory;
}

void* PageFrameAllocator::RequestPage()
{
    for (; pageBitmapIndex < PageBitMap.Size * 8; pageBitmapIndex++)
    {
        if (PageBitMap[pageBitmapIndex])
            continue;
        LockPage((void*)(pageBitmapIndex * 4096));
        return(void*)(pageBitmapIndex * 4096);
    }
    
    return NULL; // Page Frame Swap to file
}

void PageFrameAllocator::FreePage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (!PageBitMap[index])
        return;
    if(!PageBitMap.Set(index, false))
        return;
    if (pageBitmapIndex > index)
        pageBitmapIndex = index;
    freeMemory += 4096;
    usedMemory -= 4096;
}

void PageFrameAllocator::LockPage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitMap[index])
        return;
    if(!PageBitMap.Set(index, true))
        return;
    freeMemory -= 4096;
    usedMemory += 4096;
}

void PageFrameAllocator::FreePages(void* address, uint64_t pageCount)
{
    for (int i = 0; i < pageCount; i++)
        FreePage((void*)((uint64_t)address + (i * 4096)));
    
}

void PageFrameAllocator::LockPages(void* address, uint64_t pageCount)
{
    for (int i = 0; i < pageCount; i++)
        LockPage((void*)((uint64_t)address + (i * 4096)));
}

void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize)
{
    if (Initialized)
        return;
    Initialized = true;

    uint64_t mMapEntries = mMapSize / mMapDescSize;

    void* largestFreeMemSeg = NULL;
    size_t largestFreeMemSegSize = 0;

    for (int i = 0; i < mMapEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));

        if (desc->type == 7)
            if (desc->numPages * 4096 > largestFreeMemSegSize)
            {
                largestFreeMemSeg = desc->physAddr;
                largestFreeMemSegSize = desc->numPages * 4096;
            }
    
    }

    uint64_t memorySize = GetMemorySize(mMap, mMapEntries, mMapDescSize);
    freeMemory = memorySize;
    reservedMemory = 0;
    usedMemory = 0;
    uint64_t bitmapSize =  (memorySize / 4096 / 8) + 1;
    
    InitBitmap(bitmapSize, largestFreeMemSeg);

    LockPages(PageBitMap.Buffer, (PageBitMap.Size / 4096) + 1);

    for (int i = 0; i < mMapEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));

        if (desc->type != 7)
            ReservePages(desc->physAddr, desc->numPages);
    }


    //reservedMemory = data;
}