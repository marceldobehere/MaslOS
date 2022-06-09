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
    
    GlobalRenderer->Println("ERROR: NO MORE RAM AVAIABLE!", Colors.red);

    while (true);

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


    GlobalRenderer->Println("Largest Mem Size: {} Bytes.", to_string(largestFreeMemSegSize), Colors.yellow);
    GlobalRenderer->Println("Bitmap Size:      {} Bytes.", to_string(bitmapSize + sizeof(Bitmap)), Colors.yellow);

    //while(true);

    InitBitmap(bitmapSize, largestFreeMemSeg);


    //LockPages(PageBitMap.Buffer, (PageBitMap.Size / 4096) + 1);

    ReservePages(0, memorySize / 4096 + 1);

    for (int i = 0; i < mMapEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));

        if (desc->type == 7)
            UnreservePages(desc->physAddr, desc->numPages);
    }
    ReservePages(0, 0x200);
    LockPages(PageBitMap.Buffer, (PageBitMap.Size / 4096) + 1);


    GlobalRenderer->Println("Bitmap ADDR:      {}", ConvertHexToString((uint64_t)PageBitMap.Buffer), Colors.yellow);
    //reservedMemory = data;

    //while(true);
}

uint64_t PageFrameAllocator::GetFreePageCount()
{
    GlobalRenderer->Println("Size: {}", to_string(PageBitMap.Size * 8), Colors.yellow);
    GlobalRenderer->Print("Page: 0", Colors.yellow);
    uint64_t count = 0;
    for (uint64_t index = 0; index < PageBitMap.Size * 8; index++)
    {
        if (!PageBitMap[index])
            count++;
        GlobalRenderer->CursorPosition.x = 0;
        
        GlobalRenderer->Print("Page: {}    ", to_string(index), Colors.black);
        GlobalRenderer->CursorPosition.x = 0;
        GlobalRenderer->Print("Page: {}    ", to_string(index+1), Colors.silver);
    }

    //GlobalRenderer->CursorPosition.x = 0;
    //GlobalRenderer->Print("Page: {}    ", to_string(PageBitMap.Size * 8), Colors.black);
    //GlobalRenderer->CursorPosition.x = 0;

    GlobalRenderer->Println();

    return count;
}