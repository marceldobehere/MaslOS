#include "kernelUtil.h"

// extern uint64_t _KernelStart; 
// extern uint64_t _KernelEnd;





void PrepareACPI(BootInfo* bootInfo)
{
    AddToStack("PrepareACPI", "kernelUtil.cpp");
    osData.debugTerminalWindow->Log("Preparing ACPI...");
    osData.debugTerminalWindow->Log("RSDP Addr: {}", ConvertHexToString((uint64_t)bootInfo->rsdp));
    ACPI::SDTHeader* xsdt = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);
    osData.debugTerminalWindow->Log("XSDT Header Addr: {}", ConvertHexToString((uint64_t)xsdt));
    //osData.debugTerminalWindow->Log("Length: {}", to_string((uint64_t)xsdt->Length));
    int entries = (xsdt->Length - sizeof(ACPI::SDTHeader)) / 8;
    osData.debugTerminalWindow->Log("Entry count: {}", to_string(entries));

    osData.debugTerminalWindow->renderer->Print("> ");
    for (int t = 0; t < entries; t++)
    {
        ACPI::SDTHeader* newSDTHeader = (ACPI::SDTHeader*)*(uint64_t*)((uint64_t)xsdt + sizeof(ACPI::SDTHeader) + (t * 8));
        
        for (int i = 0; i < 4; i++)
            osData.debugTerminalWindow->renderer->Print(newSDTHeader->Signature[i]);

        osData.debugTerminalWindow->renderer->Print(" ");
    }
    osData.debugTerminalWindow->renderer->Println();

    RemoveFromStack();
}



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


void PrepareWindows()
{
    osData.windows = List<Window*>();

    Window* realMainWindow;
    {
        realMainWindow = (Window*)malloc(sizeof(Window));
        *(realMainWindow) = Window(NULL, Size(GlobalRenderer->framebuffer->Width, GlobalRenderer->framebuffer->Height), Position(0, 0), GlobalRenderer, "Real Main Window");
        osData.realMainWindow = realMainWindow;
    }

    Window* realMainWindow2;
    {
        realMainWindow2 = (Window*)malloc(sizeof(Window));
        *(realMainWindow2) = Window(NULL, Size(GlobalRenderer->framebuffer->Width, GlobalRenderer->framebuffer->Height), Position(0, 0), GlobalRenderer, "Real Main Window - Buffer 2");
        osData.realMainWindow2 = realMainWindow2;
    }

    Window* debugTerminalWindow;
    {
        debugTerminalWindow = (Window*)malloc(sizeof(Window));
        //TerminalInstance* terminal = (TerminalInstance*)malloc(sizeof(TerminalInstance));
        //*terminal = TerminalInstance(&adminUser, debugTerminalWindow);
        *(debugTerminalWindow) = Window(NULL /*(DefaultInstance*)terminal*/, Size(400, 600), Position(600, 20), realMainWindow->renderer, "Debug Terminal");
        //osData.windows.add(debugTerminalWindow);

        osData.debugTerminalWindow = debugTerminalWindow;
        osData.showDebugterminal = true;

        osData.debugTerminalWindow->newPosition.x = GlobalRenderer->framebuffer->Width - (osData.debugTerminalWindow->size.width + 2);
        osData.debugTerminalWindow->newPosition.y = 23;

        debugTerminalWindow->renderer->Clear(Colors.black);
        //KeyboardPrintStart(debugTerminalWindow);
        debugTerminalWindow->renderer->Println("MaslOS - Debug Terminal", Colors.green);
        debugTerminalWindow->renderer->Println("-----------------------\n", Colors.green);
        debugTerminalWindow->renderer->color = Colors.yellow;
    }
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

    InitPS2Mouse(bootInfo->mouseZIP, "default.mbif");
    //mouseImage = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(bootInfo->mouseZIP, "default.mbif"));

    InitKeyboard();

    initUsers();

    PrepareWindows();

    

    bootInfo->rsdp = (ACPI::RSDP2*)((uint64_t)bootInfo->rsdp + 20); //idk why but this is very important unless ya want the whole os to crash on boot

    PrepareACPI(bootInfo);

    outb(PIC1_DATA, 0b11111000);
    outb(PIC2_DATA, 0b11101111);

    asm ("sti");

    return kernelInfo;
}

