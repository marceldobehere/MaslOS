#pragma once
#include <stddef.h>
#include "../efiMemory.h"
#include "../bitmap.h"
#include "../memory.h"



class PageFrameAllocator{
    private:
    uint64_t freeMemory;
    uint64_t reservedMemory;
    uint64_t usedMemory;
    bool Initialized = false;
    
    void InitBitmap(size_t bitmapSize, void* bufferAddress)
    {
        PageBitMap.Size = bitmapSize;
        PageBitMap.Buffer = (uint8_t*) bufferAddress;
        for (int i = 0; i < bitmapSize; i++)
            *(uint8_t*)(PageBitMap.Buffer + i) = 0;
    }

    void ReservePage(void* address)
    {
        uint64_t index = (uint64_t)address / 4096;
        if (PageBitMap[index])
            return;
        PageBitMap.Set(index, true);
        freeMemory -= 4096;
        reservedMemory += 4096;
    }

    void UnreservePage(void* address)
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
    void ReservePages(void* address, uint64_t pageCount)
    {
        for (int i = 0; i < pageCount; i++)
            ReservePage((void*)((uint64_t)address + (i * 4096)));
    }
    void UnreservePages(void* address, uint64_t pageCount)
    {
        for (int i = 0; i < pageCount; i++)
            UnreservePage((void*)((uint64_t)address + (i * 4096)));
    }


    public:

    uint64_t GetFreeRAM()
    {
        return freeMemory;
    }
    uint64_t GetUsedRAM()
    {
        return usedMemory;
    }
    uint64_t GetReservedRAM()
    {
        return reservedMemory;
    }

    uint64_t pageBitmapIndex = 0;
    void* RequestPage()
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

    void FreePage(void* address)
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
    void LockPage(void* address)
    {
        uint64_t index = (uint64_t)address / 4096;
        if (PageBitMap[index])
            return;
        if(!PageBitMap.Set(index, true))
            return;
        freeMemory -= 4096;
        usedMemory += 4096;
    }
    void FreePages(void* address, uint64_t pageCount)
    {
        for (int i = 0; i < pageCount; i++)
            FreePage((void*)((uint64_t)address + (i * 4096)));
        
    }
    void LockPages(void* address, uint64_t pageCount)
    {
        for (int i = 0; i < pageCount; i++)
            LockPage((void*)((uint64_t)address + (i * 4096)));
    }

    Bitmap PageBitMap;
    void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize)
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

        LockPages(&PageBitMap, (PageBitMap.Size / 4096) + 1);

        for (int i = 0; i < mMapEntries; i++)
        {
            EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));

            if (desc->type != 7)
                ReservePages(desc->physAddr, desc->numPages);
        }


        //reservedMemory = data;
    }
      
};

PageFrameAllocator GlobalAllocator;

