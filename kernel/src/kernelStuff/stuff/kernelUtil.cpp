#include "kernelUtil.h"

// extern uint64_t _KernelStart; 
// extern uint64_t _KernelEnd;




void PrepareACPI(BootInfo* bootInfo)
{
    AddToStack();
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


    ACPI::MCFGHeader* mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(xsdt, (char*)"MCFG");

    osData.debugTerminalWindow->Log("MCFG Header Addr: {}", ConvertHexToString((uint64_t)mcfg));

    PCI::EnumeratePCI(mcfg);

    osData.debugTerminalWindow->Log("Drive Count: {}", to_string(osData.diskInterfaces.getCount()), Colors.yellow);
    

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
    
    SetIDTGate((void*)GenMathFault_handler, 0x0, IDT_TA_InterruptGate, 0x08); // Divide by 0
    SetIDTGate((void*)Debug_handler, 0x1, IDT_TA_InterruptGate, 0x08); // Debug
    SetIDTGate((void*)Breakpoint_handler, 0x3, IDT_TA_InterruptGate, 0x08); // Breakpoint
    SetIDTGate((void*)GenFloatFault_handler, 0x10, IDT_TA_InterruptGate, 0x08); // x87 Float error
    SetIDTGate((void*)GenFloatFault_handler, 0x13, IDT_TA_InterruptGate, 0x08); // SIMD Float error

    SetIDTGate((void*)GenFault_handler, 0x2, IDT_TA_InterruptGate, 0x08); // Non Maskable interrupt
    SetIDTGate((void*)GenFault_handler, 0x4, IDT_TA_InterruptGate, 0x08); // Overflow
    SetIDTGate((void*)GenFault_handler, 0x5, IDT_TA_InterruptGate, 0x08); // Bound Range Exceeded
    SetIDTGate((void*)GenFault_handler, 0x6, IDT_TA_InterruptGate, 0x08); // Invalid OPCODE
    SetIDTGate((void*)GenFault_handler, 0x7, IDT_TA_InterruptGate, 0x08); // Device not avaiable
    SetIDTGate((void*)GenFault_handler, 0xA, IDT_TA_InterruptGate, 0x08); // Invalid TSS
    SetIDTGate((void*)GenFault_handler, 0xB, IDT_TA_InterruptGate, 0x08); // Segment not present
    SetIDTGate((void*)GenFault_handler, 0xC, IDT_TA_InterruptGate, 0x08); // Stack segment fault
    SetIDTGate((void*)GenFault_handler, 0x11, IDT_TA_InterruptGate, 0x08); //  Alligment check
    SetIDTGate((void*)GenFault_handler, 0x12, IDT_TA_InterruptGate, 0x08); // machine check
    SetIDTGate((void*)GenFault_handler, 0x14, IDT_TA_InterruptGate, 0x08); // Virtualization Exception
    SetIDTGate((void*)GenFault_handler, 0x15, IDT_TA_InterruptGate, 0x08); // Control Protection Exception
    SetIDTGate((void*)GenFault_handler, 0x1C, IDT_TA_InterruptGate, 0x08); // Hypervisor Inhection Exception
    SetIDTGate((void*)GenFault_handler, 0xD, IDT_TA_InterruptGate, 0x08); // VMM Communication Exception


    __asm__ volatile ("lidt %0" : : "m" (idtr));
    
    
    //asm ("int $0x1");

    AddToStack();
    RemapPIC(
        0b11111000, 
        0b11101111
    );

    __asm__ volatile ("sti");
    RemoveFromStack();
    
}

void PrepareWindowsTemp(Framebuffer* img)
{
    VirtualRenderer::psf1_font = GlobalRenderer->psf1_font;

    osData.windows = List<Window*>();
    osData.windowPointerThing = (WindowManager::WindowPointerBufferThing*)malloc(sizeof(WindowManager::WindowPointerBufferThing), "Alloc WindowPointerBufferThing");
    *osData.windowPointerThing = WindowManager::WindowPointerBufferThing(GlobalRenderer->framebuffer, img, Colors.blue);
}

void PrepareWindows(Framebuffer* img)
{
    // VirtualRenderer::psf1_font = GlobalRenderer->psf1_font;

    // osData.windows = List<Window*>();
    // osData.windowPointerThing = (WindowManager::WindowPointerBufferThing*)malloc(sizeof(WindowManager::WindowPointerBufferThing), "Alloc WindowPointerBufferThing");
    // *osData.windowPointerThing = WindowManager::WindowPointerBufferThing(GlobalRenderer->framebuffer, img, Colors.blue);

    // Window* realMainWindow;
    // {
    //     realMainWindow = (Window*)malloc(sizeof(Window));
    //     *(realMainWindow) = Window(NULL, Size(GlobalRenderer->framebuffer->Width, GlobalRenderer->framebuffer->Height), Position(0, 0), GlobalRenderer, "Real Main Window");
    //     osData.realMainWindow = realMainWindow;
    // }

    // Window* realMainWindow2;
    // {
    //     realMainWindow2 = (Window*)malloc(sizeof(Window));
    //     *(realMainWindow2) = Window(NULL, Size(GlobalRenderer->framebuffer->Width, GlobalRenderer->framebuffer->Height), Position(0, 0), GlobalRenderer, "Real Main Window - Buffer 2");
    //     osData.realMainWindow2 = realMainWindow2;
    // }

    Window* debugTerminalWindow;
    {
        debugTerminalWindow = (Window*)malloc(sizeof(Window), "Debug Terminal Window");
        DebugTerminalInstance* dterminal = (DebugTerminalInstance*)malloc(sizeof(DebugTerminalInstance), "Debug Terminal Instance");
        *dterminal = DebugTerminalInstance(debugTerminalWindow);
        
        *(debugTerminalWindow) = Window(dterminal, Size(500, 800), Position(600, 20), "Debug Terminal", true, true, true);
        //osData.windows.add(debugTerminalWindow);

        osData.debugTerminalWindow = debugTerminalWindow;
        osData.showDebugterminal = true;

        osData.debugTerminalWindow->newPosition.x = GlobalRenderer->framebuffer->Width - (osData.debugTerminalWindow->size.width + 2);
        osData.debugTerminalWindow->newPosition.y = 1;

        debugTerminalWindow->renderer->Clear(Colors.black);
        //KeyboardPrintStart(debugTerminalWindow);
        debugTerminalWindow->renderer->Println("MaslOS - Debug Terminal (OUTPUT ONLY)", Colors.green);
        debugTerminalWindow->renderer->Println("-------------------------------------\n", Colors.green);
        debugTerminalWindow->renderer->color = Colors.yellow;
    }
}



BasicRenderer r = *((BasicRenderer*)NULL);
KernelInfo InitializeKernel(BootInfo* bootInfo)
{
    AddToStack();
    r = BasicRenderer(bootInfo->framebuffer, bootInfo->psf1_font);
    GlobalRenderer = &r;
    //osData.realMainWindow->framebuffer = r.framebuffer;

    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);
    
    PrepareMemory(bootInfo);

    _memset(bootInfo->framebuffer->BaseAddress, 0, bootInfo->framebuffer->BufferSize);

    InitializeHeap((void*)0x0000100000000000, 0x10);

    Framebuffer* bgImg = kernelFiles::ConvertImageToFramebuffer(bootInfo->bgImage);

    PrepareWindowsTemp(bgImg);


    

    PIT::InitPIT();



    InitPS2Mouse(bootInfo->mouseZIP, "default.mbif");
    //mouseImage = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(bootInfo->mouseZIP, "default.mbif"));

    InitKeyboard();

    PrepareInterrupts();

    initUsers();

    osData.diskInterfaces = List<DiskInterface::GenericDiskInterface*>();

    osData.windowIconZIP = bootInfo->windowIconsZIP;
    osData.windowButtonZIP = bootInfo->windowButtonZIP;

    for (int i = 0; i < WindowManager::countOfWindowIcons; i++)
        WindowManager::internalWindowIcons[i] = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(osData.windowIconZIP, WindowManager::windowIconNames[i]));
    
    for (int i = 0; i < WindowManager::countOfButtonIcons; i++)
    {
        //osData.debugTerminalWindow->Log("Loading Window {}.", to_string(i), Colors.yellow);
        //osData.debugTerminalWindow->Log("- Name: \"{}\"", WindowManager::windowIconNames[i], Colors.yellow);
        WindowManager::windowButtonIcons[i] = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(osData.windowButtonZIP, WindowManager::windowButtonIconNames[i]));
        //osData.debugTerminalWindow->Log("- ADDR 1: {}", ConvertHexToString((uint64_t)WindowManager::windowIcons[i]), Colors.yellow);
        //osData.debugTerminalWindow->Log("- Width:  {}px", to_string(WindowManager::windowIcons[i]->width), Colors.yellow);
        //osData.debugTerminalWindow->Log("- Height: {}px", to_string(WindowManager::windowIcons[i]->height), Colors.yellow);
    }
    
    PrepareWindows(bgImg);




    
    Taskbar::InitTaskbar(bootInfo->MButton, bootInfo->MButtonS);

    bootInfo->rsdp = (ACPI::RSDP2*)((uint64_t)bootInfo->rsdp + 20); //idk why but this is very important unless ya want the whole os to crash on boot

    PrepareACPI(bootInfo);


    

    RemoveFromStack();
    return kernelInfo;
}

