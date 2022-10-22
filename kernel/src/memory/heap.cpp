#include "heap.h"
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"
#include "../OSDATA/osdata.h"


const uint32_t HeapMagicNum = 2406789212;

int64_t heapCount = 0;
int64_t usedHeapCount = 0;
void* heapStart;
void* heapEnd;
HeapSegHdr* lastHdr;

int64_t mallocCount = 0;
int64_t freeCount = 0;
int64_t activeMemFlagVal = 0;


void HeapSegHdr::CombineForward()
{
    AddToStack();
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
    text = "<FREE>";
    heapCount--;
    //GlobalRenderer->Print(">");
    RemoveFromStack();
}

void HeapSegHdr::CombineBackward()
{
    AddToStack();
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
    AddToStack();

    //AddToStack();
    if (splitLength < 0x10)
    {
        //RemoveFromStack();
        RemoveFromStack();
        return NULL;
    }

    //AddToStack();
    //GlobalRenderer->Println("this len: {}", to_string(length), Colors.bgreen);
    int64_t splitSegLength = (length - splitLength) - sizeof(HeapSegHdr);
    //GlobalRenderer->Println("Splitseg len: {}", to_string(splitSegLength), Colors.bgreen);
    if (splitSegLength < 0x10)
    {
        //RemoveFromStack();
        //RemoveFromStack();
        RemoveFromStack();
        return NULL;
    }

    HeapSegHdr* newSplitHdr = (HeapSegHdr*)((uint64_t)this + splitLength + sizeof(HeapSegHdr));
    //AddToStack();
    *newSplitHdr = HeapSegHdr();

    //GlobalRenderer->Println("Splitheader addr: {}", ConvertHexToString((uint64_t)newSplitHdr), Colors.bgreen);
    //AddToStack();
    if (next != NULL)
        next->last = newSplitHdr;
    //AddToStack();

    //AddToStack();
    newSplitHdr->next = next;
    next = newSplitHdr;
    //AddToStack();
    newSplitHdr->last = this;
    newSplitHdr->length = splitSegLength;
    newSplitHdr->free = free;
    newSplitHdr->magicNum = HeapMagicNum;
    length = splitLength;
    newSplitHdr->text = "<FREE>";

    //GlobalRenderer->Println("this len: {}", to_string(length), Colors.bgreen);

    //AddToStack();
    if (lastHdr == this) 
        lastHdr = newSplitHdr;
    //GlobalRenderer->Println("Split successful!");
    heapCount++;

    //RemoveFromStack();
    //RemoveFromStack();
    //RemoveFromStack();
    //RemoveFromStack();
    //RemoveFromStack();
    //RemoveFromStack();
    //RemoveFromStack();
    RemoveFromStack();
    return newSplitHdr;
}



void InitializeHeap(void* heapAddress, size_t pageCount)
{
    AddToStack();
    void* pos = heapAddress;

    activeMemFlagVal = 0;

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
    startSeg->text = "<FREE>";
    startSeg->magicNum = HeapMagicNum;
    startSeg->activeMemFlagVal = activeMemFlagVal;
    lastHdr = startSeg;
    heapCount = 1;
    usedHeapCount = 0;

    RemoveFromStack();
}

void* malloc(size_t size) 
{
    return malloc(size, "<NO TEXT GIVEN>");
}

uint64_t mCount = 0;

void* malloc(size_t size, const char* text)
{
    mCount++;

GlobalRenderer->CursorPosition = {400, 160};
GlobalRenderer->Print("HALLO: ", Colors.red); 

    //AddToStack();
    


    if (size == 0)
        size = 0x10;

    if (size % 0x10 > 0)
    {
        size -= (size % 0x10);
        size += 0x10;
    }

GlobalRenderer->CursorPosition = {300, 20};
for (int i = 0; i < 20; i++)
    GlobalRenderer->delChar(GlobalRenderer->CursorPosition.x + i * 8, GlobalRenderer->CursorPosition.y, Colors.black);
GlobalRenderer->Print("SIZE: {}", to_string(size), Colors.bgreen); 
GlobalRenderer->CursorPosition = {300, 0};
for (int i = 0; i < 40; i++)
    GlobalRenderer->delChar(GlobalRenderer->CursorPosition.x + i * 8, GlobalRenderer->CursorPosition.y, Colors.black);
GlobalRenderer->Print("MALLOC COUNT: {}", to_string(mCount), Colors.bgreen); 
GlobalRenderer->CursorPosition = {460, 0};
for (int i = 0; i < 30; i++)
    GlobalRenderer->delChar(GlobalRenderer->CursorPosition.x + i * 8, GlobalRenderer->CursorPosition.y, Colors.black);
GlobalRenderer->Print("FREE RAM: {}", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen); 
GlobalRenderer->CursorPosition = {800, 0};
for (int i = 0; i < 30; i++)
    GlobalRenderer->delChar(GlobalRenderer->CursorPosition.x + i * 8, GlobalRenderer->CursorPosition.y, Colors.black);
GlobalRenderer->Print("STACK COUNT: {}", to_string(MStackData::stackPointer), Colors.bgreen); 

    HeapSegHdr* current = (HeapSegHdr*) heapStart;
    while(true)
    {

        if ((uint64_t)current < 10000)
            Panic("CURRENT IS NULL BRO", true);

GlobalRenderer->CursorPosition = {400, 160};
GlobalRenderer->Print("HALLO: ", Colors.silver); 

        if (current->magicNum != HeapMagicNum)
        {
            Panic("Trying to access invalid HeapSegment Header!", true);
            //RemoveFromStack();
            return NULL;
        }

GlobalRenderer->CursorPosition = {400, 160};
GlobalRenderer->Print("HALLO: ", Colors.orange); 

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
                current->text = text;
                current->activeMemFlagVal = activeMemFlagVal;
                mallocCount++;
                usedHeapCount++;
                //RemoveFromStack();

GlobalRenderer->CursorPosition = {400, 160};
GlobalRenderer->Print("HALLO: ", Colors.bblue); 

                return (void*)((uint64_t)current + sizeof(HeapSegHdr));
            }
            if (current->length == size)
            {
                current->free = false;
                current->text = text;
                current->activeMemFlagVal = activeMemFlagVal;
                mallocCount++;
                usedHeapCount++;
                //RemoveFromStack();

GlobalRenderer->CursorPosition = {400, 160};
GlobalRenderer->Print("HALLO: ", Colors.bgreen); 

                return (void*)((uint64_t)current + sizeof(HeapSegHdr));
            }
        }

GlobalRenderer->CursorPosition = {400, 160};
GlobalRenderer->Print("HALLO: ", Colors.brown); 

        if (current->next == NULL)
            break;
        current = current->next;
    }
    //GlobalRenderer->Println("Requesting more RAM.");

GlobalRenderer->CursorPosition = {400, 160};
GlobalRenderer->Print("HALLO: ", Colors.cyan); 

    if (ExpandHeap(size))
    {
        void* res = malloc(size, text);
        //mallocCount++;
        //RemoveFromStack();

GlobalRenderer->CursorPosition = {400, 160};
GlobalRenderer->Print("HALLO: ", Colors.yellow); 

        return res;
    }

    // GlobalRenderer->ClearDotted(Colors.green);
    // while (true);

GlobalRenderer->CursorPosition = {400, 160};
GlobalRenderer->Print("HALLO: ", Colors.bgreen); 


    Panic("MALLOC FAILED!!!");

    //RemoveFromStack();
    return NULL;
}

void free(void* address)
{
    AddToStack();
    HeapSegHdr* segment = ((HeapSegHdr*)address) - 1;

    if (segment->magicNum == HeapMagicNum)
    {
        if (!segment->free)
        {
            freeCount++;
            segment->free = true;
            segment->text = "<FREE>";
            //GlobalRenderer->Print("A");
            //GlobalRenderer->Print("<");
            segment->CombineForward();
            //GlobalRenderer->Print("-");
            segment->CombineBackward();
            //GlobalRenderer->Print(">");
            usedHeapCount--;
        }
        else
        {
            Panic("Tried to free already free Segment!");
            RemoveFromStack();
            return;
        }
    }
    else
    {
        Panic("Tried to free invalid Segment!");
        RemoveFromStack();
        return;
    }
    RemoveFromStack();
}

void* _malloc(size_t size)
{
    AddToStack();
    void* res = malloc(size);
    RemoveFromStack();
    return res;
}

void _free(void* address)
{
    AddToStack();
    free(address);
    RemoveFromStack();
}

void* _malloc(size_t size, const char* text)
{
    AddToStack();
    void* res = malloc(size, text);
    RemoveFromStack();
    return res;
}


bool ExpandHeap(size_t length)
{
    AddToStack();
    if (length % 0x1000)
    {
        length -= (length % 0x1000);
        length += 0x1000;
    }

    size_t pageCount = length / 0x1000;
    HeapSegHdr* newSegment = (HeapSegHdr*) heapEnd;
    void* tHeapEnd = heapEnd;

    //GlobalRenderer->Println("Page Count  {}", to_string(pageCount), Colors.white);
    //GlobalRenderer->Println("free RAM 1: {}", to_string(GlobalAllocator->GetFreeRAM()), Colors.white);



    for (size_t i = 0; i < pageCount; i++)
    {
        void* tempAddr = GlobalAllocator->RequestPage();
        if (tempAddr == NULL)
            Panic("NO MORE RAM!!!!!!!", true);
        
        GlobalPageTableManager.MapMemory(tHeapEnd, tempAddr);
        tHeapEnd = (void*)((size_t)tHeapEnd + 0x1000);
    }



    newSegment->free = true;
    newSegment->last = lastHdr;
    lastHdr->next = newSegment;
    lastHdr = newSegment;
    newSegment->magicNum = HeapMagicNum;
    
    newSegment->next = NULL;
    newSegment->text = "<FREE>";
    newSegment->length = (pageCount * 0x1000) - sizeof(HeapSegHdr);
    heapCount++;
    
    heapEnd = tHeapEnd;







    // for (size_t i = 0; i < pageCount; i++)
    // {
    //     if (i == 1)
    //     {
    //         newSegment->free = true;
    //         newSegment->last = lastHdr;
    //         lastHdr->next = newSegment;
    //         lastHdr = newSegment;
    //         newSegment->magicNum = HeapMagicNum;
            
    //         newSegment->next = NULL;
    //         newSegment->text = "<FREE>";
    //         newSegment->length = 0x1000 - sizeof(HeapSegHdr);
    //         heapCount++;
    //         //newSegment->CombineBackward(); 
    //         //GlobalRenderer->Println("ADD MEM", Colors.yellow);
    //     }
    //     if (i > 0)
    //     {
    //         uint64_t tempI = (i+1) * 0x1000;
    //         newSegment->length = tempI - sizeof(HeapSegHdr);
    //     }

    //     void* tempAddr = GlobalAllocator->RequestPage();
    //     if (tempAddr == NULL)
    //     {
    //         //GlobalRenderer->Println("<HEAP START>", Colors.yellow);
    //         // we gotta add the stuff that we requested but didnt add because it return NULL here
            
    //         // GlobalRenderer->ClearDotted(Colors.bblue);
    //         // while(true);
    //         Panic("NO MORE RAM!!!!!!!", true);

    //         RemoveFromStack();
    //         //GlobalRenderer->Println("<HEAP END>", Colors.yellow);
    //         return false;
    //     }
    //     GlobalPageTableManager.MapMemory(heapEnd, tempAddr);
    //     heapEnd = (void*)((size_t)heapEnd + 0x1000);
    // }

    //GlobalRenderer->Println("free RAM 2: {}", to_string(GlobalAllocator->GetFreeRAM()), Colors.white);
    
    // newSegment->free = true;
    // newSegment->last = lastHdr;
    // lastHdr->next = newSegment;
    // lastHdr = newSegment;
    // newSegment->magicNum = HeapMagicNum;

    if (newSegment == NULL)
        Panic("NEW SEGMENT IS NULL!");

    // newSegment->next = NULL;
    // newSegment->text = "<FREE>";
    newSegment->length = length - sizeof(HeapSegHdr);
    // heapCount++;
    newSegment->CombineBackward();
    RemoveFromStack();
    return true;
}
