#include "heap.h"
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"
#include "../OSDATA/osdata.h"


const uint32_t HeapMagicNum = 2406789212;

int64_t heapCount = 0;
void* heapStart;
void* heapEnd;
HeapSegHdr* lastHdr;



void HeapSegHdr::CombineForward()
{
    AddToStack("CombineForward", "memory/heap.cpp");
    //GlobalRenderer->Print("B");
    //GlobalRenderer->Print("[{}]", ConvertHexToString((uint64_t)this), Colors.yellow);
    if (next == NULL)
    {
        RemoveFromStack();
        return;   
    }
    //GlobalRenderer->Print("[{}]", ConvertHexToString((uint64_t)next), Colors.yellow);
    if (!next->free)
    {
        RemoveFromStack();
        return;   
    }
    //GlobalRenderer->Print("B2");
    
    //GlobalRenderer->Print("<");
    if (next == lastHdr) 
        lastHdr = this;
    //GlobalRenderer->Print("-");
    if (next->next != NULL)
    {
        next->next->last = this;
    }
    //GlobalRenderer->Print("-");
    length = length + next->length + sizeof(HeapSegHdr);
    //GlobalRenderer->Print("-");
    next = next->next;
    heapCount--;
    //GlobalRenderer->Print(">");
    RemoveFromStack();
}

void HeapSegHdr::CombineBackward()
{
    AddToStack("CombineBackward", "memory/heap.cpp");
    //GlobalRenderer->Print("C");
    if (last != NULL)
        if (last->free)
        {
            //GlobalRenderer->Print("<");
            last->CombineForward();
            //GlobalRenderer->Print(">");
        }
    RemoveFromStack();
}

HeapSegHdr* HeapSegHdr::Split(size_t splitLength)
{
    AddToStack("Split", "memory/heap.cpp");

    AddToStack("Split 1", "memory/heap.cpp");
    if (splitLength < 0x10)
    {
        RemoveFromStack();
        RemoveFromStack();
        return NULL;
    }

    AddToStack("Split 2", "memory/heap.cpp");
    //GlobalRenderer->Println("this len: {}", to_string(length), Colors.bgreen);
    int64_t splitSegLength = (length - splitLength) - sizeof(HeapSegHdr);
    //GlobalRenderer->Println("Splitseg len: {}", to_string(splitSegLength), Colors.bgreen);
    if (splitSegLength < 0x10)
    {
        RemoveFromStack();
        RemoveFromStack();
        RemoveFromStack();
        return NULL;
    }

    HeapSegHdr* newSplitHdr = (HeapSegHdr*)((uint64_t)this + splitLength + sizeof(HeapSegHdr));
    //AddToStack(to_string((uint64_t)newSplitHdr), "memory/heap.cpp"));
    *newSplitHdr = HeapSegHdr();

    //GlobalRenderer->Println("Splitheader addr: {}", ConvertHexToString((uint64_t)newSplitHdr), Colors.bgreen);
    AddToStack("Split 3", "memory/heap.cpp");
    if (next != NULL)
        next->last = newSplitHdr;
    //AddToStack(to_string((uint64_t)newSplitHdr), "memory/heap.cpp"));

    AddToStack("Split 4", "memory/heap.cpp");
    newSplitHdr->next = next;
    next = newSplitHdr;
    AddToStack("Split 5", "memory/heap.cpp");
    newSplitHdr->last = this;
    newSplitHdr->length = splitSegLength;
    newSplitHdr->free = free;
    newSplitHdr->magicNum = HeapMagicNum;
    length = splitLength;

    //GlobalRenderer->Println("this len: {}", to_string(length), Colors.bgreen);

    AddToStack("Split 6", "memory/heap.cpp");
    if (lastHdr == this) 
        lastHdr = newSplitHdr;
    //GlobalRenderer->Println("Split successful!");
    heapCount++;

    RemoveFromStack();
    RemoveFromStack();
    RemoveFromStack();
    RemoveFromStack();
    //RemoveFromStack();
    RemoveFromStack();
    RemoveFromStack();
    RemoveFromStack();
    return newSplitHdr;
}



void InitializeHeap(void* heapAddress, size_t pageCount)
{
    AddToStack("InitializeHeap", "memory/heap.cpp");
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
    startSeg->magicNum = HeapMagicNum;
    lastHdr = startSeg;
    heapCount = 1;

    RemoveFromStack();
}

void* malloc(size_t size)
{
    AddToStack("malloc", "memory/heap.cpp");
    
    if (size % 0x10 > 0)
    {
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0)
    {
        RemoveFromStack();
        return NULL;
    }

    HeapSegHdr* current = (HeapSegHdr*) heapStart;
    while(true)
    {
        if (current->magicNum != HeapMagicNum)
        {
            Panic("Trying to access invalid HeapSegment Header!");
        }
        if (current->free)
        {
            if (current->length > size)
            {
                if (current->Split(size) == NULL)
                {
                    if (current->next == NULL)
                        break;

                    current = current->next;
                    continue;
                }
                current->free = false;
                RemoveFromStack();
                return (void*)((uint64_t)current + sizeof(HeapSegHdr));
            }
            if (current->length == size)
            {
                current->free = false;
                RemoveFromStack();
                return (void*)((uint64_t)current + sizeof(HeapSegHdr));
            }
        }
        if (current->next == NULL)
            break;
        current = current->next;
    }
    //GlobalRenderer->Println("Requesting more RAM.");
    ExpandHeap(size);
    void* res = malloc(size);
    RemoveFromStack();
    return res;
}

void free(void* address)
{
    AddToStack("free", "memory/heap.cpp");
    HeapSegHdr* segment = ((HeapSegHdr*)address) - 1;

    if (segment->magicNum == HeapMagicNum)
    {
        if (!segment->free)
        {
            segment->free = true;
            //GlobalRenderer->Print("A");
            //GlobalRenderer->Print("<");
            segment->CombineForward();
            //GlobalRenderer->Print("-");
            segment->CombineBackward();
            //GlobalRenderer->Print(">");
        }
        else
        {
            Panic("Tried to free already free Segment!");
        }
    }
    else
    {
        Panic("Tried to free invalid Segment!");
    }
    RemoveFromStack();
}

void* _malloc(size_t size)
{
    AddToStack("_malloc", "memory/heap.cpp");
    void* res = malloc(size);

    RemoveFromStack();
    return res;
}

void _free(void* address)
{
    AddToStack("_free", "memory/heap.cpp");
    free(address);
    RemoveFromStack();
}

void ExpandHeap(size_t length)
{
    AddToStack("ExpandHeap", "memory/heap.cpp");
    if (length % 0x1000)
    {
        length -= (length % 0x1000);
        length += 0x1000;
    }

    size_t pageCount = length / 0x1000;
    HeapSegHdr* newSegment = (HeapSegHdr*) heapEnd;

    //GlobalRenderer->Println("Page Count  {}", to_string(pageCount), Colors.white);
    //GlobalRenderer->Println("free RAM 1: {}", to_string(GlobalAllocator->GetFreeRAM()), Colors.white);

    for (size_t i = 0; i < pageCount; i++)
    {
        GlobalPageTableManager.MapMemory(heapEnd, GlobalAllocator->RequestPage());
        heapEnd = (void*)((size_t)heapEnd + 0x1000);
    }

    //GlobalRenderer->Println("free RAM 2: {}", to_string(GlobalAllocator->GetFreeRAM()), Colors.white);
    
    newSegment->free = true;
    newSegment->last = lastHdr;
    lastHdr->next = newSegment;
    lastHdr = newSegment;
    newSegment->magicNum = HeapMagicNum;

    newSegment->next = NULL;
    newSegment->length = length - sizeof(HeapSegHdr);
    newSegment->CombineBackward();
    heapCount++;
    RemoveFromStack();
}