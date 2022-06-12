#include "kernelUtil.h"

// extern uint64_t _KernelStart;
// extern uint64_t _KernelEnd;




KernelInfo kernelInfo;
static PageFrameAllocator t = PageFrameAllocator();

void PrepareMemory(BootInfo* bootInfo)
{
    uint64_t mMapEntries = (bootInfo->mMapSize / bootInfo->mMapDescSize);

    GlobalAllocator = &t;
    GlobalAllocator->ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize); 
    
    uint64_t kernelSize = (((uint64_t)&_KernelEnd) - ((uint64_t)&_KernelStart));
    uint64_t kernelPages = ((uint64_t)kernelSize / 4096) + 1;

    GlobalAllocator->LockPages(&_KernelStart, kernelPages);

    PageTable* PML4 = (PageTable*)GlobalAllocator->RequestPage();
    memset(PML4, 0, 0x1000);
    GlobalPageTableManager = PageTableManager(PML4);

    for (uint64_t i = 0; i < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize); i+=0x1000)
        GlobalPageTableManager.MapMemory((void*)i, (void*)i);
    
    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
    GlobalAllocator->LockPages((void*)fbBase, fbSize / 0x1000);
    
    for (uint64_t i = fbBase; i < fbBase + fbSize; i+=4096)
        GlobalPageTableManager.MapMemory((void*)i, (void*)i);


    asm("mov %0, %%cr3" : : "r" (PML4) );

    kernelInfo.pageTableManager = &GlobalPageTableManager;
}

IDTR idtr;

void SetIDTGate(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector)
{
    IDTDescEntry* interrupt = (IDTDescEntry*)(idtr.Offset + entryOffset * sizeof(IDTDescEntry));
    interrupt->SetOffset((uint64_t)handler);
    interrupt->type_attr = type_attr;
    interrupt->selector = selector;
}

void PrepareInterrupts()
{  
    idtr.Limit = 0x0FFF;
    idtr.Offset = (uint64_t)GlobalAllocator->RequestPage();

    SetIDTGate((void*)PageFault_handler, 0xE, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)DoubleFault_handler, 0x8, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)GPFault_handler, 0xD, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)KeyboardInt_handler, 0x21, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)MouseInt_handler, 0x2C, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)PITInt_handler, 0x20, IDT_TA_InterruptGate, 0x08);
    

    asm("lidt %0" : : "m" (idtr));

    RemapPIC();
}




BasicRenderer r = *((BasicRenderer*)NULL);
KernelInfo InitializeKernel(BootInfo* bootInfo)
{
    r = BasicRenderer(bootInfo->framebuffer, bootInfo->psf1_font);
    GlobalRenderer = &r;
    //osData.realMainWindow->framebuffer = r.framebuffer;

    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);
    
    PrepareMemory(bootInfo);

    memset(bootInfo->framebuffer->BaseAddress, 0, bootInfo->framebuffer->BufferSize);

    InitializeHeap((void*)0x0000100000000000, 0x10);

    PIT::InitPIT();

    PrepareInterrupts();

    InitPS2Mouse();

    InitKeyboard();

    initUsers();

    outb(PIC1_DATA, 0b11111000);
    outb(PIC2_DATA, 0b11101111);

    asm ("sti");

    return kernelInfo;
}

