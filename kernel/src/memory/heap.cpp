#include "heap.h"
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"

void* heapStart;
void* heapEnd;
HeapSegHdr* lastHdr;

void HeapSegHdr::CombineForward()
{
    if (next == NULL)
        return;
    if (!next->free)
        return;
    
    if (next == lastHdr) 
        lastHdr = this;

    if (next->next != NULL)
    {
        next->next->last = this;
    }

    length = length + next->length + sizeof(HeapSegHdr);


}

void HeapSegHdr::CombineBackward()
{
    if (last != NULL && last->free)
        last->CombineForward();
}

HeapSegHdr* HeapSegHdr::Split(size_t splitLength)
{
    if (splitLength < 0x10)
        return NULL;

    int64_t splitSegLength = length - splitLength - sizeof(HeapSegHdr);

    if (splitSegLength < 0x10)
        return NULL;

    HeapSegHdr* newSplitHdr = (HeapSegHdr*)((size_t)this + splitLength + sizeof(HeapSegHdr));

    next->last = newSplitHdr;
    newSplitHdr->next = next;
    next = newSplitHdr;
    newSplitHdr->last = this;
    newSplitHdr->length = splitSegLength;
    newSplitHdr->free = free;
    length = splitLength;

    if (lastHdr == this) lastHdr = newSplitHdr;

    return newSplitHdr;
}



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

    heapStart = heapAddress;
    heapEnd = (void*)((size_t)heapStart + heapLength);
    HeapSegHdr* startSeg = (HeapSegHdr*)heapAddress;
    startSeg->length = heapLength - sizeof(HeapSegHdr);
    startSeg->next = NULL;
    startSeg->last = NULL;
    startSeg->free = true;
    lastHdr = startSeg;
}

void* malloc(size_t size)
{
    if (size % 0x10 > 0)
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
        if (current->next == NULL)
            break;
        current = current->next;
    }

    ExpandHeap(size);
    return malloc(size);
}

void free(void* address)
{
    HeapSegHdr* segment = (HeapSegHdr*)address - 1;
    segment->free = true;
    segment->CombineForward();
    segment->CombineBackward();
}

void ExpandHeap(size_t length)
{
    if (length % 0x1000)
    {
        length -= (length % 0x1000);
        length += 0x1000;
    }

    size_t pageCount = length / 0x1000;
    HeapSegHdr* newSegment = (HeapSegHdr*) heapEnd;

    for (size_t i = 0; i < pageCount; i++)
    {
        GlobalPageTableManager.MapMemory(heapEnd, GlobalAllocator->RequestPage());
        heapEnd = (void*)((size_t)heapEnd + 0x1000);
    }

    newSegment->free = true;
    newSegment->last = lastHdr;
    lastHdr->next = newSegment;
    lastHdr = newSegment;

    newSegment->next = NULL;
    newSegment->length = length - sizeof(HeapSegHdr);
    newSegment->CombineBackward();
}