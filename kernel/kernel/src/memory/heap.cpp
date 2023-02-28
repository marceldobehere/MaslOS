#include "heap.h"
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"
#include "../OSDATA/osdata.h"


const uint32_t HeapMagicNum = 2406789212;

int64_t lastFreeSize = 0;
int64_t heapCount = 0;
int64_t usedHeapCount = 0;
void* heapStart;
void* heapEnd;
HeapSegHdr* lastHdr;

int64_t mallocCount = 0;
int64_t freeCount = 0;
int64_t activeMemFlagVal = 0;

bool heapInited = false;

void* MallocCache16BytesAddr [MallocCacheCount];
bool MallocCache16BytesFree [MallocCacheCount];
uint64_t MallocCache16BytesTime [MallocCacheCount];



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
    int64_t splitSegLength = ((int64_t)length - (int64_t)splitLength) - (int64_t)sizeof(HeapSegHdr);
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


int64_t lastUpdateTime = 0;
bool mallocToCache = false;


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
        GlobalPageTableManager.MapMemory(pos, GlobalAllocator->RequestPage(), true);
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

    heapInited = true;

    mallocToCache = true;
    for (int i = 0; i < MallocCacheCount; i++)
    {
        MallocCache16BytesFree[i] = true;
        MallocCache16BytesAddr[i] = _Malloc(MallocCacheSize, "Init malloc cache");
        MallocCache16BytesTime[i] = 0;
        if (MallocCache16BytesAddr[i] == NULL)
            Panic("Malloc Cache is NULL", true);
    }
    mallocToCache = false;
    lastUpdateTime = 0;

    RemoveFromStack();
}



void UpdateMallocCache()
{
    if (mallocToCache)
        Panic("MALLOC TO CACHE IS ON!", true);
    
    if (!PIT::Inited)
        return;

    AddToStack();
    uint64_t now = PIT::TimeSinceBootMS();
    if (now < lastUpdateTime + 200)
    {
        RemoveFromStack();
        return;
    }

    lastUpdateTime = now;

    mallocToCache = true;
    for (int i = 0; i < MallocCacheCount; i++)
    {
        if (!MallocCache16BytesFree[i] && now > MallocCache16BytesTime[i] + 400)
        {
            MallocCache16BytesAddr[i] = _Malloc(MallocCacheSize, "Malloc cache 2");
            MallocCache16BytesTime[i] = now;
            MallocCache16BytesFree[i] = true;
        }
    }
    mallocToCache = false;

    RemoveFromStack();
}





void* _Xmalloc(size_t size, const char* func, const char* file, int line)
{
    return _Xmalloc(size, "<NO TEXT GIVEN>", func, file, line);
}

uint64_t mCount = 0;





bool HeapCheck(bool wait)
{
    AddToStack();
    GlobalRenderer->Clear(Colors.black);
    GlobalRenderer->Println("> Performing Heap Check...", Colors.white);
    if (wait)
        PIT::Sleep(100);
    bool foundError = false;

    HeapSegHdr* current = (HeapSegHdr*) heapStart;
    int counter = 0;
    while(true)
    {
        counter++;
        if (GlobalRenderer->CursorPosition.y >= ((GlobalRenderer->framebuffer->Height * 3) / 4))
        {
            //PIT::Sleep(200);
            GlobalRenderer->Clear(Colors.black);
            GlobalRenderer->Println("> Heap Check:", Colors.white);
        }
        GlobalRenderer->Println("<Heapseg: {}>", to_string(counter), Colors.yellow);

        if ((uint64_t)current < 10000)
        {
            GlobalRenderer->Println("*Heapseg is at NULL!", Colors.bred);
            GlobalRenderer->Println("-> Heapseg addr: 0x{}", ConvertHexToString((uint64_t)current), Colors.bred);
            break;
        }

        if (current->magicNum != HeapMagicNum)
        {
            GlobalRenderer->Println("*Heapseg is invalid!", Colors.bred);
            GlobalRenderer->Println("-> Heapseg addr: 0x{}", ConvertHexToString((uint64_t)current), Colors.bred);
            break;
        }

        if (current->next == NULL)
            break;
        
        if (((uint64_t)(current->length + sizeof(HeapSegHdr) + (uint64_t)current)) != (uint64_t)current->next)
        {
            GlobalRenderer->Print("* Heapseg at 0x{} ", ConvertHexToString((uint64_t)current), Colors.bred);
            GlobalRenderer->Print("with size: {} ", to_string(current->length), Colors.bred);
            GlobalRenderer->Print("+ {} ", to_string(sizeof(HeapSegHdr)), Colors.bred);
            GlobalRenderer->Println("= 0x{}", ConvertHexToString(((uint64_t)(current->length + sizeof(HeapSegHdr) + (uint64_t)current))), Colors.bred);
            GlobalRenderer->Println("-> Points to 0x{}!", ConvertHexToString((uint64_t)current->next), Colors.bred);

            GlobalRenderer->Println();
            break;
        }

        if (current->last != NULL)
        {
            if (current->last->next != current)
            {
                GlobalRenderer->Println("*Heapseg prev is not pointing to current!", Colors.bred);
                GlobalRenderer->Println("-> Heapseg at addr: 0x{}", ConvertHexToString((uint64_t)current), Colors.bred);
                GlobalRenderer->Println("-> Prev Heapseg at addr: 0x{}", ConvertHexToString((uint64_t)current->last), Colors.bred);
                break;
            }
        }


        current = current->next;
    }


    GlobalRenderer->Println("> Heap Check Done!", Colors.white);
    if (foundError)
    {
        GlobalRenderer->Println("> Heap has Errors!", Colors.bred);
        while (true)
            ;
        
        RemoveFromStack();
        return false;
    }
    if (wait)
        PIT::Sleep(500);
    
    RemoveFromStack();
    return true;
}


void* _Xmalloc(size_t size, const char* text, const char* func, const char* file, int line)
{
    mCount++;
    AddToStack();
    if (!heapInited)
        Panic("Trying to malloc with Heap not being initialized!", true);


    if (size == 0)
        size = 0x10;

    if (size % 0x10 > 0)
    {
        size -= (size % 0x10);
        size += 0x10;
    }

    if (!mallocToCache)
        UpdateMallocCache();

    if (size == MallocCacheSize && !mallocToCache && PIT::Inited)
    {
        for (int i = 0; i < MallocCacheCount; i++)
        {
            if (MallocCache16BytesFree[i])
            {
                MallocCache16BytesFree[i] = false;
                MallocCache16BytesTime[i] = PIT::TimeSinceBootMS();

                if (MallocCache16BytesAddr[i] < (void*)100)
                    Panic("Malloc Cache Thing is NULL", true);
                
                HeapSegHdr* tempHdr = ((HeapSegHdr*)MallocCache16BytesAddr[i]) - 1;
                tempHdr->text = "<BYTE CACHE>";
                tempHdr->activeMemFlagVal = activeMemFlagVal;
                tempHdr->file = text;
                tempHdr->func = file;
                tempHdr->line = line;
                tempHdr->time = PIT::TimeSinceBootMS();


                RemoveFromStack();
                return MallocCache16BytesAddr[i];
            }
        }
    }



    HeapSegHdr* current = (HeapSegHdr*) heapStart;
    while(true)
    {

        if ((uint64_t)current < 10000)
            Panic("CURRENT IS NULL BRO", true); 

        if (current->magicNum != HeapMagicNum)
        {
            Panic("Trying to access invalid HeapSegment Header!", true);
            RemoveFromStack();
            return NULL;
        }

        if (current->free)
        {
            if (current->length > (size + sizeof(HeapSegHdr) + 0x10))
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
                current->file = file;
                current->func = func;
                current->line = line;
                current->time = PIT::TimeSinceBootMS();
                mallocCount++;
                usedHeapCount++;
                RemoveFromStack();
                return (void*)((uint64_t)current + sizeof(HeapSegHdr));
            }
            if (current->length == size)
            {
                current->free = false;
                current->text = text;
                current->activeMemFlagVal = activeMemFlagVal;
                current->file = file;
                current->func = func;
                current->line = line;
                current->time = PIT::TimeSinceBootMS();
                mallocCount++;
                usedHeapCount++;
                RemoveFromStack();
                return (void*)((uint64_t)current + sizeof(HeapSegHdr));
            }
        }

        if (current->next == NULL)
            break;
        current = current->next;
    }
    //GlobalRenderer->Println("Requesting more RAM.");

    if (ExpandHeap(size))
    {
        void* res = _Malloc(size, text);
        //mallocCount++;
        RemoveFromStack();

        return res;
    }

    // GlobalRenderer->ClearDotted(Colors.green);
    // while (true);


    Panic("MALLOC FAILED!!!", true);

    RemoveFromStack();
    return NULL;
}




void _Xfree(void* address, const char* func, const char* file, int line)
{
    AddToStack();
    if (!heapInited)
        Panic("Trying to free with Heap not being initialized!", true);
    if (mallocToCache)
        Panic("Malloc to cache on when trying to free!");
    
    if (address < (void*)1000)
        Panic("Tried to free NULL address!", true);
    HeapSegHdr* segment = ((HeapSegHdr*)address) - 1;

    if (segment->magicNum == HeapMagicNum)
    {

        if (segment->length == MallocCacheSize)
        {
            for (int i = 0; i < MallocCacheCount; i++)
            {
                if (MallocCache16BytesAddr[i] == address)
                {
                    if (!MallocCache16BytesFree[i])
                    {
                        MallocCache16BytesFree[i] = true;
                        //MallocCache16BytesTime[i] = PIT::TimeSinceBootMS();
                        RemoveFromStack();
                        return;
                    }
                    else
                    {
                        Panic("Tried to free already free cache!");
                        RemoveFromStack();
                        return;
                    }
                }
            }
        }


        if (!segment->free)
        {
            lastFreeSize = segment->length;
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
            RemoveFromStack();
            return;
        }
        else
        {
            Panic("Tried to free already free Segment!", true);
            RemoveFromStack();
            return;
        }
    }
    else
    {
        Panic("Tried to free invalid Segment!", true);
        RemoveFromStack();
        return;
    }
    RemoveFromStack();
}

/*

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

*/




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
        
        GlobalPageTableManager.MapMemory(tHeapEnd, tempAddr, true);
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
        Panic("NEW SEGMENT IS NULL!", true);

    // newSegment->next = NULL;
    // newSegment->text = "<FREE>";
    newSegment->length = length - sizeof(HeapSegHdr);
    // heapCount++;
    newSegment->CombineBackward();
    RemoveFromStack();
    return true;
}

bool _XtryFree(void* address, const char* func, const char* file, int line)
{
    if (address  < (void*)1000)
        return false;

    AddToStack();

    if (!heapInited)
        Panic("Trying to free with Heap not being initialized!", true);
    if (mallocToCache)
        Panic("Malloc to cache on when trying to free!");


    HeapSegHdr* segment = ((HeapSegHdr*)address) - 1;

    if (segment->magicNum == HeapMagicNum)
    {

        if (segment->length == MallocCacheSize)
        {
            for (int i = 0; i < MallocCacheCount; i++)
            {
                if (MallocCache16BytesAddr[i] == address)
                {
                    if (!MallocCache16BytesFree[i])
                    {
                        MallocCache16BytesFree[i] = true;
                        //MallocCache16BytesTime[i] = PIT::TimeSinceBootMS();
                        RemoveFromStack();
                        return true;
                    }
                    else
                    {
                        //Panic("Tried to free already free Segment!");
                        RemoveFromStack();
                        return false;
                    }
                }
            }
        }




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

            RemoveFromStack();
            return true;
        }
        else
        {
            //Panic("Tried to free already free Segment!");
            RemoveFromStack();
            return false;
        }
    }
    else
    {
        //Panic("Tried to free invalid Segment!");
        RemoveFromStack();
        return false;
    }
    RemoveFromStack();
    return true;
}





