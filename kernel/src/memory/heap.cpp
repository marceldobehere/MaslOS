#include "heap.h"
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"

void HeapSegHdr::CombineForward()
{

}

void HeapSegHdr::CombineBackward()
{

}

HeapSegHdr* HeapSegHdr::Split(size_t splitLength)
{





    return NULL;
}

void* heapStart;
void* heapEnd;
HeapSegHdr* lastHdr;

void InitializeHeap(void* heapAddress, size_t pageCount)
{
    void* pos = heapAddress;

    for (size_t i = 0; i < pageCount; i++)
    {
        //uint64_t addr = (uint64_t)GlobalAllocator->RequestPage();
        //GlobalRenderer->Println("Requesting Page: {}", ConvertHexToString(addr), Colors.yellow);
        //GlobalPageTableManager.MapMemory(pos, (void*)addr);
        GlobalPageTableManager.MapMemory(pos, GlobalAllocator->RequestPage());
        pos = (void*)((size_t)pos + 0x1000);
    }

    size_t heapLength = pageCount * 0x1000;

    // heapStart = heapAddress;
    // heapEnd = (void*)((size_t)heapStart + heapLength);
    // HeapSegHdr* startSeg = (HeapSegHdr*)heapAddress;
    // startSeg->length = heapLength - sizeof(HeapSegHdr);
    // startSeg->next = NULL;
    // startSeg->last = NULL;
    // startSeg->free = true;
    // lastHdr = startSeg;
}

void* malloc(size_t size)
{
    if (size & 0x10 > 0)
    {
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0)
        return NULL;

    HeapSegHdr* current = (HeapSegHdr*) heapStart;
    while(true)
    {
        if (current->free)
        {
            if (current->length > size)
            {
                current->Split(size);
                current->free = false;
                return (void*)((uint64_t)current + sizeof(HeapSegHdr));
            }
            if (current->length == size)
            {
                current->free = false;
                return (void*)((uint64_t)current + sizeof(HeapSegHdr));
            }
        }
        if (current->next)
            break;
        current = current->next;
    }

    ExpandHeap(size);
    return malloc(size);
}

void free(void* address)
{

}

void ExpandHeap(size_t length)
{

}