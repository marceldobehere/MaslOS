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
    
    void InitBitmap(size_t bitmapSize, void* bufferAddress);

    void ReservePage(void* address);

    void UnreservePage(void* address);

    void ReservePages(void* address, uint64_t pageCount);

    void UnreservePages(void* address, uint64_t pageCount);


    public:

    Bitmap PageBitMap;

    uint64_t GetFreeRAM();

    uint64_t GetFreePageCount();

    uint64_t GetUsedRAM();

    uint64_t GetReservedRAM();

    uint64_t pageBitmapIndex = (uint64_t)0;
    void* RequestPage();

    void FreePage(void* address);

    void LockPage(void* address);

    void FreePages(void* address, uint64_t pageCount);

    void LockPages(void* address, uint64_t pageCount);

    void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize);
      
};


extern PageFrameAllocator* GlobalAllocator;