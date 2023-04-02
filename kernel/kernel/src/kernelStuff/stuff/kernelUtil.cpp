#include "kernelUtil.h"

uint64_t _KernelStart; 
uint64_t _KernelEnd;

#include "../other_IO/acpi/acpiShutdown.h"

// #include "../../interrupts/panic.h"

#include "../other_IO/rtc/rtc.h"


void DoPCIWithoutMCFG(BootInfo* bootInfo)
{

}


void PrepareACPI(BootInfo* bootInfo)
{
    AddToStack();

    

    AddToStack();
    osData.debugTerminalWindow->Log("Preparing ACPI...");
    osData.debugTerminalWindow->Log("RSDP Addr: {}", ConvertHexToString((uint64_t)bootInfo->rsdp));
    RemoveFromStack();

    AddToStack();   
    ACPI::SDTHeader* rootThing = NULL;
    int div = 1;


    if (bootInfo->rsdp->firstPart.Revision == 0)
    {
        osData.debugTerminalWindow->Log("ACPI Version: 1");
        rootThing = (ACPI::SDTHeader*)(uint64_t)(bootInfo->rsdp->firstPart.RSDTAddress);
        osData.debugTerminalWindow->Log("RSDT Header Addr: {}", ConvertHexToString((uint64_t)rootThing));
        div = 4;

        if (rootThing == NULL)
        {
            Panic("RSDT Header is at NULL!", true);
        }
        else
        {
            //GlobalRenderer->Clear(Colors.black);
            PrintMsg("> Testing ACPI Loader...");

            InitAcpiShutdownThing(rootThing);
            //while (true);
        }
    }
    else
    {
        osData.debugTerminalWindow->Log("ACPI Version: 2");
        rootThing = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);
        osData.debugTerminalWindow->Log("XSDT Header Addr: {}", ConvertHexToString((uint64_t)rootThing));
        div = 8;

        if (rootThing == NULL)
        {
            Panic("XSDT Header is at NULL!", true);
        }
    }
    

    RemoveFromStack();

    
    AddToStack();
    int entries = (rootThing->Length - sizeof(ACPI::SDTHeader)) / div;
    osData.debugTerminalWindow->Log("Entry count: {}", to_string(entries));
    RemoveFromStack();

    AddToStack();
    osData.debugTerminalWindow->renderer->Print("> ");
    for (int t = 0; t < entries; t++)
    {
        uint64_t bleh1 = *(uint64_t*)((uint64_t)rootThing + sizeof(ACPI::SDTHeader) + (t * div));
        if (div == 4)
            bleh1 &= 0x00000000FFFFFFFF;
        ACPI::SDTHeader* newSDTHeader = (ACPI::SDTHeader*)bleh1;
        
        for (int i = 0; i < 4; i++)
            osData.debugTerminalWindow->renderer->Print(newSDTHeader->Signature[i]);

        osData.debugTerminalWindow->renderer->Print(" ");
    }
    osData.debugTerminalWindow->renderer->Println();
    RemoveFromStack();

    AddToStack();
    ACPI::MCFGHeader* mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(rootThing, (char*)"MCFG", div);

    osData.debugTerminalWindow->Log("MCFG Header Addr: {}", ConvertHexToString((uint64_t)mcfg));
    RemoveFromStack();

    if (mcfg == NULL)
    {
        //Panic("MCFG HEADER IS NULL!", true);
        DoPCIWithoutMCFG(bootInfo);

        RemoveFromStack();
        return;
    }


    // for (int i = 0; i < 20; i++)
    //     GlobalRenderer->Clear(Colors.purple);

    //Panic("bruh", true);

    AddToStack();
    PCI::EnumeratePCI(mcfg);
    RemoveFromStack();

    // for (int i = 0; i < 20; i++)
    //     GlobalRenderer->Clear(Colors.bblue);

    AddToStack();
    osData.debugTerminalWindow->Log("Drive Count: {}", to_string(osData.diskInterfaces.getCount()), Colors.yellow);
    RemoveFromStack();    
    
    // for (int i = 0; i < 20; i++)
    //     GlobalRenderer->Clear(Colors.orange);

    RemoveFromStack();
}



KernelInfo kernelInfo;
static PageFrameAllocator t = PageFrameAllocator();

#define USER_END            0x0000007fffffffff
#define TRANSLATED_PHYSICAL_MEMORY_BEGIN 0xffff800000000000llu
#define MMIO_BEGIN          0xffffffff00000000llu
#define KERNEL_TEMP_BEGIN   0xffffffff40000000llu
#define KERNEL_DATA_BEGIN   0xffffffff80000000llu
#define KERNEL_HEAP_BEGIN   0xffffffff80300000llu

static inline uint64_t earlyVirtualToPhysicalAddr(const void* vAddr)
{
    if((0xfffff00000000000llu & (uint64_t)vAddr) == TRANSLATED_PHYSICAL_MEMORY_BEGIN)
        return ~TRANSLATED_PHYSICAL_MEMORY_BEGIN & (uint64_t)vAddr;
    else
        return ~KERNEL_DATA_BEGIN & (uint64_t)vAddr;
}

void PrepareMemory(BootInfo* bootInfo)
{
    GlobalAllocator = &t;
    PrintMsgStartLayer("EFI Memory Map");
    GlobalAllocator->ReadEFIMemoryMap(bootInfo->mMapStart, bootInfo->mMapSize); 
    PrintMsgEndLayer("EFI Memory Map");

    

    //uint64_t kernelSize = (((uint64_t)&_KernelEnd) - ((uint64_t)&_KernelStart));
    //uint64_t kernelPages = ((uint64_t)kernelSize / 4096) + 1;

    //GlobalAllocator->LockPages(&_KernelStart, kernelPages);

    uint64_t rFB = earlyVirtualToPhysicalAddr(GlobalRenderer->framebuffer->BaseAddress);

    PageTable* PML4 = (PageTable*)GlobalAllocator->RequestPage();
    //GlobalPageTableManager.MapMemory(PML4, PML4);
    _memset(PML4, 0, 0x1000);
    PrintMsg("> Getting PML4 Stuff");
    //GlobalRenderer->Println("PML4 ADDR:          {}", ConvertHexToString((uint64_t)PML4), Colors.yellow);
    asm volatile("mov %%cr3, %0" : "=r"(PML4));
    // asm("mov %0, %%cr3" : : "r" (PML4) );
    PrintMsgStartLayer("Info");
    PrintMsgCol("PML4 ADDR:          {}", ConvertHexToString((uint64_t)PML4), Colors.yellow);
    PrintMsgCol("FB 1 ADDR:          {}", ConvertHexToString(rFB), Colors.yellow);
    PrintMsgCol("FB 2 ADDR:          {}", ConvertHexToString((uint64_t)GlobalRenderer->framebuffer->BaseAddress), Colors.yellow);
    PrintMsgCol("FB 3 ADDR:          {}", ConvertHexToString((uint64_t)bootInfo->framebuffer->BaseAddress), Colors.yellow);
    PrintMsgCol("MMAP ADDR:          {}", ConvertHexToString((uint64_t)bootInfo->mMapStart), Colors.yellow);
    PrintMsgCol("KERNEL ADDR:        {}", ConvertHexToString((uint64_t)bootInfo->kernelStart), Colors.yellow);
    PrintMsgCol("KERNEL V ADDR:      {}", ConvertHexToString((uint64_t)bootInfo->kernelStartV), Colors.yellow);
    GlobalPageTableManager = PageTableManager(PML4);
    PrintMsgEndLayer("Info");

    
    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize;
    //GlobalAllocator->LockPages((void*)earlyVirtualToPhysicalAddr((void*)fbBase), fbSize / 0x1000);

    PrintMsg("> Mapping Framebuffer Memory");
    if (true)
    {
        //for (uint64_t i = fbBase; i < fbBase + fbSize; i+=4096)
        //    GlobalPageTableManager.MapFramebufferMemory((void*)i, (void*)i);
    }
    else
    {
        for (int i = 0; i < 50; i++)
            GlobalRenderer->Clear(Colors.red);

        for (uint64_t i = fbBase; i < fbBase + fbSize; i+=4096)
            GlobalPageTableManager.MapFramebufferMemory((void*)i, (void*)i);

        for (int i = 0; i < 50; i++)
            GlobalRenderer->Clear(Colors.green);

        doMagicWithPAT();

        for (int i = 0; i < 50; i++)
            GlobalRenderer->Clear(Colors.purple);

        GlobalRenderer->Clear(Colors.black);
    }

    

    // while (true)
    //     ;


    int32_t testing[0x1000/4];
    testing[0] = 123;

    // GlobalRenderer->Println("TEST 1:      {}", to_string(testing[0]), Colors.yellow);
    // GlobalPageTableManager.MapMemory((void*)0xFF00FF00FF00FF00, (void*)testing);
    // int32_t* testing2 = (int32_t*)0xFF00FF00FF00FF00;
    // GlobalRenderer->Println("TEST 2:      {}", to_string(testing2[0]), Colors.yellow);

    // while(true);


/*

    for (uint64_t i = (uint64_t)bootInfo->kernelStart; i < (uint64_t)bootInfo->kernelStart + bootInfo->kernelSize; i+=0x1000)
       GlobalPageTableManager.MapMemory((void*)i, (void*)i);

    for (uint64_t i = (uint64_t)bootInfo->m2MapStart; i < (uint64_t)bootInfo->mMapStart + bootInfo->mMapSize + 0x1000; i+=0x1000)
       GlobalPageTableManager.MapMemory((void*)i, (void*)i);
    

    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize;
    //GlobalAllocator->LockPages((void*)rFB, fbSize / 0x1000);
    
    for (uint64_t i = fbBase; i < fbBase + fbSize  + 0x1000; i+=4096)
        GlobalPageTableManager.MapMemory((void*)i, (void*)i);

*/

    // for (int i = 0; i < 20; i++)
    //     GlobalRenderer->Clear(Colors.yellow);

       //GlobalPageTableManager.MapMemory( (void*)earlyVirtualToPhysicalAddr((void*)i), (void*)earlyVirtualToPhysicalAddr((void*)i));

    // for (uint64_t i = (uint64_t)bootInfo->kernelStart; i < (uint64_t)bootInfo->kernelStart + bootInfo->kernelSize + 0x1000; i+=4096)
    //    GlobalPageTableManager.MapMemory((void*)i, (void*)i);

    // for (uint64_t i = (uint64_t)bootInfo->kernelStartV; i < (uint64_t)bootInfo->kernelStartV + bootInfo->kernelSize + 0x1000; i+=4096)
    //    GlobalPageTableManager.MapMemory((void*)i, (void*)i);
       //GlobalPageTableManager.MapMemory((void*)((uint64_t)bootInfo->kernelStartV + i), (void*)i);


    // for (uint64_t i = (uint64_t)bootInfo->mMapStart; i < (uint64_t)bootInfo->mMapStart + bootInfo->mMapSize; i+=0x1000)
    //     GlobalPageTableManager.MapMemory((void*)i, (void*)i);


    //asm("mov %0, %%cr3" : : "r" (PML4) );

    //while(true);
    //asm volatile("movq %[cr3_var], %%cr3;"::[cr3_var]"r"((uint64_t)PML4));

    //while(true);

    PrintMsg("> Setting Page Table Manager");
    kernelInfo.pageTableManager = &GlobalPageTableManager;

    //while(true);

    /*
    // {
    //     uint64_t s = (uint64_t)GlobalRenderer->framebuffer->BaseAddress;
    //     uint64_t e = s + GlobalRenderer->framebuffer->BufferSize;
    //     s -= s % 0x1000;
    //     for (uint64_t i = s; i < e; i+=0x1000)
    //         GlobalPageTableManager.MapMemory((void*)i, (void*)i);
    //         //GlobalPageTableManager.MapMemory((void*)i, (void*)earlyVirtualToPhysicalAddr((void*)i));
    // }

    
    //while(true);


    for (uint64_t i = (uint64_t)bootInfo->mMapStart - ((uint64_t)bootInfo->mMapStart) % 0x1000; i < (uint64_t)bootInfo->mMapStart + bootInfo->mMapSize; i+=0x1000)
        GlobalPageTableManager.MapMemory((void*)i, (void*)i);

    //while(true);
    
    
    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
    //GlobalAllocator->LockPages((void*)earlyVirtualToPhysicalAddr((void*)fbBase), fbSize / 0x1000);


    for (uint64_t i = fbBase; i < fbBase + fbSize; i+=4096)
        GlobalPageTableManager.MapMemory((void*)i, (void*)i);

    //while(true);

    //asm("mov %0, %%cr3" : : "r" (PML4) );

    kernelInfo.pageTableManager = &GlobalPageTableManager;

    while(true);
    */
}

uint8_t testIdtrArr[0x1000];
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
    idtr.Limit = 0x1000 - 1;

    for (int i = 0; i < 0x1000; i++)
        testIdtrArr[i] = 0;

    idtr.Offset = (uint64_t)testIdtrArr;//(uint64_t)GlobalAllocator->RequestPage();

    // GenericInt_handler
    for (int i = 0; i < (0x1000 / sizeof(IDTDescEntry)); i++)
        SetIDTGate((void*)GenericInt_handler, i, IDT_TA_InterruptGate, 0x08);

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
    SetIDTGate((void*)InvalidOpCode_handler, 0x6, IDT_TA_InterruptGate, 0x08); // Invalid OPCODE
    SetIDTGate((void*)GenFault_handler, 0x7, IDT_TA_InterruptGate, 0x08); // Device not avaiable
    SetIDTGate((void*)GenFault1_handler, 0xA, IDT_TA_InterruptGate, 0x08); // Invalid TSS
    SetIDTGate((void*)GenFault1_handler, 0xB, IDT_TA_InterruptGate, 0x08); // Segment not present
    SetIDTGate((void*)GenFault1_handler, 0xC, IDT_TA_InterruptGate, 0x08); // Stack segment fault
    SetIDTGate((void*)GenFault1_handler, 0x11, IDT_TA_InterruptGate, 0x08); //  Alligment check
    SetIDTGate((void*)GenFault1_handler, 0x12, IDT_TA_InterruptGate, 0x08); // machine check
    SetIDTGate((void*)VirtualizationFault_handler, 0x14, IDT_TA_InterruptGate, 0x08); // Virtualization Exception
    SetIDTGate((void*)ControlProtectionFault_handler, 0x15, IDT_TA_InterruptGate, 0x08); // Control Protection Exception
    SetIDTGate((void*)HypervisorFault_handler, 0x1C, IDT_TA_InterruptGate, 0x08); // Hypervisor Injection Exception
    SetIDTGate((void*)VMMCommunicationFault_handler, 0x1D, IDT_TA_InterruptGate, 0x08); // VMM Communication Exception
    SetIDTGate((void*)SecurityException_handler, 0x1E, IDT_TA_InterruptGate, 0x08); // Security Exception

    io_wait();    
    __asm__ volatile ("lidt %0" : : "m" (idtr));
    io_wait();    
    //asm ("int $0x1");

    AddToStack();
    RemapPIC(
        0b11111000, //0b11111000, 
        0b11101111 //0b11101111
    );
    
    io_wait();    
    __asm__ volatile ("sti");
    RemoveFromStack();
    
}

void PrepareWindowsTemp(Framebuffer* img)
{
    VirtualRenderer::psf1_font = GlobalRenderer->psf1_font;

    
    osData.windows = List<Window*>();
    osData.windowsToGetActive = List<Window*>();
    
    osData.windowPointerThing = (WindowManager::WindowPointerBufferThing*)_Malloc(sizeof(WindowManager::WindowPointerBufferThing), "Alloc WindowPointerBufferThing");
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
        debugTerminalWindow = (Window*)_Malloc(sizeof(Window), "Debug Terminal Window");
        DebugTerminalInstance* dterminal = (DebugTerminalInstance*)_Malloc(sizeof(DebugTerminalInstance), "Debug Terminal Instance");
        *dterminal = DebugTerminalInstance(debugTerminalWindow);
        


        *(debugTerminalWindow) = Window(dterminal, Size(1000, 3000), Position(600, 20), "Debug Terminal", true, true, true);
        //osData.windows.add(debugTerminalWindow);



        osData.debugTerminalWindow = debugTerminalWindow;
        osData.showDebugterminal = true;

        osData.debugTerminalWindow->newPosition.x = GlobalRenderer->framebuffer->Width - 500;
        osData.debugTerminalWindow->newPosition.y = 1;

        

        debugTerminalWindow->renderer->Clear(Colors.black);
        //KeyboardPrintStart(debugTerminalWindow);
        debugTerminalWindow->renderer->Println("MaslOS - Debug Terminal (OUTPUT ONLY)", Colors.green);
        debugTerminalWindow->renderer->Println("-------------------------------------\n", Colors.green);
        debugTerminalWindow->renderer->color = Colors.yellow;
    }
}

void set_fpu_cw(const uint16_t cw) 
{
	asm volatile("fldcw %0" :: "m"(cw));
}

/**
 * Enable the FPU
 *
 * We are assuming that we have one to begin with, but since we
 * only really operate on 686 machines, we do, so we're not
 * going to bother checking.
 */
void enable_fpu() 
{
	size_t cr4;
	asm volatile ("mov %%cr4, %0" : "=r"(cr4));
	cr4 |= 0x200;
	asm volatile ("mov %0, %%cr4" :: "r"(cr4));
	set_fpu_cw(0x37F);
}

#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/button/buttonComponent.h"

#include "../../tasks/enterHandler/taskEnterHandler.h"

void StartMenuButtonClick(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::MouseClickEventInfo info)
{
    if (comp->id >= 1001 && comp->id <= 1005)
    {
        // PONG
        const char* BLEHUS_TITLE = "App Terminal Window";
        const char* BLEHUS_CMD   = "echo \"Unknown App lol\"";
        bool BLEHUS_CLOSE = true;

        if (comp->id == 1001)
        {
            BLEHUS_TITLE = "Pong Terminal Window";
            BLEHUS_CMD   = "maab \"bruh:pong.maab\"";
        }

        if (comp->id == 1002)
        {
            BLEHUS_TITLE = "Mandelbrot Window";
            BLEHUS_CMD   = "maab \"bruh:maalbrot.maab\"";
            BLEHUS_CLOSE = false;
        }

        if (comp->id == 1003)
        {
            BLEHUS_TITLE = "Alert Window";
            BLEHUS_CMD   = "maab \"bruh:alert.maab\"";
        }

        if (comp->id == 1004)
        {
            BLEHUS_TITLE = "Terminal Window";
            BLEHUS_CMD   = "";
            BLEHUS_CLOSE = false;
        }

        if (comp->id == 1005)
        {
            BLEHUS_TITLE = "Task Manager Window";
            BLEHUS_CMD   = "taskmgr";
        }



        //Window* oldActive = activeWindow;
        Window* mainWindow = (Window*)_Malloc(sizeof(Window), "App Window");
        TerminalInstance* terminal = (TerminalInstance*)_Malloc(sizeof(TerminalInstance), "App Terminal");
        *terminal = TerminalInstance(&guestUser);
        *(mainWindow) = Window((DefaultInstance*)terminal, Size(500, 500), Position(50, 50), BLEHUS_TITLE, true, true, true);
        osData.windows.add(mainWindow);
        terminal->SetWindow(mainWindow);
        terminal->closeWindowAfterTask = BLEHUS_CLOSE;
        ((TerminalInstance*)mainWindow->instance)->Cls();
        //KeyboardPrintStart(mainWindow);
        //((TerminalInstance*)mainWindow->instance)->KeyboardPrintStart();

        osData.windowsToGetActive.add(mainWindow);

        //((NewTerminalInstance*)terminal->newTermInstance)->Println(BLEHUS_CMD);
        {
            int i = 0;
            for (; BLEHUS_CMD[i] != 0; i++)
                terminal->terminalInput[i] = BLEHUS_CMD[i];
            terminal->terminalInput[i] = 0;
            terminal->userlen = i;
        }
        terminal->tasks.add(NewEnterTask(terminal));
    }
}


void InitStartMenuWindow(BootInfo* bootInfo)
{
    AddToStack();
    GuiInstance* testGui;
    GuiComponentStuff::BoxComponent* box;
    GuiComponentStuff::RectangleComponent* testRect;

    int sW = 200;
    int sH = 250;

    {
        Window* window = (Window*)_Malloc(sizeof(Window), "GUI Window");
        osData.startMenuWindow = window;
        GuiInstance* gui = (GuiInstance*)_Malloc(sizeof(GuiInstance), "GUI Instance");
        *gui = GuiInstance(window);
        *(window) = Window((DefaultInstance*)gui, Size(sW, sH), Position(1, GlobalRenderer->framebuffer->Height - sH - osData.windowPointerThing->taskbar->Height), "Start Menu", false, true, false);
        window->moveToFront = true;
        window->hidden = true;
        window->resizeable = false;
        window->selectedBorderColor = Colors.orange;
        osData.windows.add(window);
        
        gui->Init();
        gui->screen->backgroundColor = Colors.black;

        testGui = gui;


        {
            GuiComponentStuff::TextComponent* txt = new GuiComponentStuff::TextComponent
            (
                testGui->screen,
                Colors.tblack, Colors.white, 
                "Start Menu", 
                GuiComponentStuff::Position(0, 10)
            );
            txt->position.x = (sW - txt->GetActualComponentSize().FixedX) / 2;

            testGui->screen->children->add(txt);
        }
    

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Pong.maab", 
            Colors.cyan, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white, 
            GuiComponentStuff::ComponentSize(80, 20),
            GuiComponentStuff::Position(0, 40), testGui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1001;
            
            testGui->screen->children->add(btn);
        }
        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Mandelbrot.maab", 
            Colors.cyan, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white, 
            GuiComponentStuff::ComponentSize(128, 20),
            GuiComponentStuff::Position(0, 60), testGui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1002;
            
            testGui->screen->children->add(btn);
        }
        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Alert.maab", 
            Colors.cyan, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white, 
            GuiComponentStuff::ComponentSize(88, 20),
            GuiComponentStuff::Position(0, 80), testGui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1003;
            
            testGui->screen->children->add(btn);
        }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("New Terminal", 
            Colors.bgray, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(104, 20),
            GuiComponentStuff::Position(0, 120), testGui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1004;
            
            testGui->screen->children->add(btn);
        }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Task Manager", 
            Colors.bgray, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(104, 20),
            GuiComponentStuff::Position(0, 140), testGui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1005;
            
            testGui->screen->children->add(btn);
        }

    }
    RemoveFromStack();
}










#include "../Disk_Stuff/Disk_Interfaces/ram/ramDiskInterface.h"
#include "../Disk_Stuff/Partition_Interfaces/mraps/mrapsPartitionInterface.h"
#include "../Disk_Stuff/Filesystem_Interfaces/mrafs/mrafsFileSystemInterface.h"

BasicRenderer r = *((BasicRenderer*)NULL);




KernelInfo InitializeKernel(BootInfo* bootInfo)
{
    AddToStack();

    _memset(bootInfo->framebuffer->BaseAddress, 0x00, bootInfo->framebuffer->BufferSize);

    r = BasicRenderer(bootInfo->framebuffer, bootInfo->psf1_font);
    GlobalRenderer = &r;


    PrintMsg("> Initializing Kernel");
    Println();
    PrintMsgStartLayer("BOOT");
    // PrintMsg("Test 1");
    // PrintMsgStartLayer("TEST");
    // PrintMsg("Test 2");

    // PrintMsgStartLayer("TEST2");
    // PrintMsg("Test 3");
    // PrintMsg("Test 4");
    // PrintMsgColSL("Test 5", Colors.red);
    // PrintMsgCol(" :)", Colors.green);
    // PrintMsgEndLayer("TEST2");

    // PrintMsg("Test 5");

    // PrintMsgEndLayer("TEST");
    // PrintMsg("Test 6");


    //GlobalRenderer->Clear(Colors.green);

    //while (true);

    //osData.realMainWindow->framebuffer = r.framebuffer;

    AddToStack();

    PrintMsg("> Preparing GDT");
    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);


    RemoveFromStack();

    

    //while (true);

    AddToStack();
    PrintMsgStartLayer("Prepare Memory");
    PrepareMemory(bootInfo);
    PrintMsgEndLayer("Prepare Memory");
    RemoveFromStack();
    
    //while(true);
    
    PrintMsg("> Initializing Heap");
    InitializeHeap((void*)0x0000100000000000, 0x10);

    //GlobalRenderer->Println("BG IMG: {}", to_string((uint64_t)bootInfo->bgImage), Colors.orange);


    

    //uint8_t* bleh = (uint8_t*) malloc(sizeof(Framebuffer), "Converting Image to Framebuffer");;

    //*bleh = 5;

    

    PrintMsg("> Getting Background Image");
    Framebuffer* bgImg = kernelFiles::ConvertImageToFramebuffer(bootInfo->bgImage);

    
    
    PrintMsg("> Preparing Windows (1/2)");
    PrepareWindowsTemp(bgImg);

    

    PrintMsg("> Initing RTC");
    RTC::InitRTC();
    PrintMsg("> Reading RTC");
    RTC::read_rtc();

    PrintMsg("> Updating RTC Time");
    RTC::UpdateTimeIfNeeded();

    PrintMsgStartLayer("RTC Info");
    PrintMsgColSL("TIME: ", Colors.yellow);
    PrintMsgColSL("{}:", to_string((int)RTC::Hour), Colors.yellow);
    PrintMsgColSL("{}:", to_string((int)RTC::Minute), Colors.yellow);
    PrintMsgCol("{}", to_string((int)RTC::Second), Colors.yellow);
    
    PrintMsgColSL("DATE: ", Colors.yellow);
    PrintMsgColSL("{}.", to_string((int)RTC::Day), Colors.yellow);
    PrintMsgColSL("{}.", to_string((int)RTC::Month), Colors.yellow);
    PrintMsgCol("{}", to_string((int)RTC::Year), Colors.yellow);
    PrintMsgEndLayer("RTC Info");

    PrintMsg("> Initing PIT");
    PIT::InitPIT();
    

    #define STAT 0x64
    #define CMD 0x60
    
    PrintMsg("> Clearing Input Buffer (1/2)");
    {
        // Clear the input buffer.
        size_t timeout = 1024;
        while ((inb(STAT) & 1) && timeout > 0) {
            timeout--;
            inb(CMD);
        }
    }
    
    PrintMsg("> Initing PS/2 Mouse");
    InitPS2Mouse(bootInfo->mouseZIP, "default.mbif");
    //mouseImage = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(bootInfo->mouseZIP, "default.mbif"));

    PrintMsg("> Initing Keyboard State List");
    InitKeyboardListRam();

    PrintMsg("> Initing PS/2 Keyboard");
    InitKeyboard();
    
    PrintMsg("> Preparing Interrupts");
    PrepareInterrupts();


    PrintMsg("> Clearing Input Buffer (2/2)");
    {
        // Clear the input buffer.
        size_t timeout = 1024;
        while ((inb(STAT) & 1) && timeout > 0) {
            timeout--;
            inb(CMD);
        }
    }
    
    PrintMsg("> Initing Users");
    initUsers();


    PrintMsg("> Creating List for Disk Interfaces");
    osData.diskInterfaces = List<DiskInterface::GenericDiskInterface*>();

    osData.windowIconZIP = bootInfo->windowIconsZIP;
    osData.windowButtonZIP = bootInfo->windowButtonZIP;

    //GlobalRenderer->Clear(Colors.black);
    //GlobalRenderer->Println("                                     COUNT OF WINDOW ICONS: {}", to_string(WindowManager::countOfWindowIcons), Colors.yellow);


    PrintMsg("> Loading Window Icons");
    for (int i = 0; i < WindowManager::countOfWindowIcons; i++)
        WindowManager::internalWindowIcons[i] = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(osData.windowIconZIP, WindowManager::windowIconNames[i]));
    

    PrintMsg("> Loading Window Button Icons");
    for (int i = 0; i < WindowManager::countOfButtonIcons; i++)
    {
        //osData.debugTerminalWindow->Log("Loading Window {}.", to_string(i), Colors.yellow);
        //osData.debugTerminalWindow->Log("- Name: \"{}\"", WindowManager::windowIconNames[i], Colors.yellow);
        WindowManager::windowButtonIcons[i] = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(osData.windowButtonZIP, WindowManager::windowButtonIconNames[i]));
        //osData.debugTerminalWindow->Log("- ADDR 1: {}", ConvertHexToString((uint64_t)WindowManager::windowIcons[i]), Colors.yellow);
        //osData.debugTerminalWindow->Log("- Width:  {}px", to_string(WindowManager::windowIcons[i]->width), Colors.yellow);
        //osData.debugTerminalWindow->Log("- Height: {}px", to_string(WindowManager::windowIcons[i]->height), Colors.yellow);
    }
    

    PrintMsg("> Preparing Windows (2/2)");
    PrepareWindows(bgImg);
    PrintDebugTerminal();
    
    PrintMsg("> Initing Taskbar");
    Taskbar::InitTaskbar(bootInfo->MButton, bootInfo->MButtonS);

    //bootInfo->rsdp = (ACPI::RSDP2*)((uint64_t)bootInfo->rsdp + 20); //idk why but this is very important unless ya want the whole os to crash on boot

    PrintMsg("> Initing Start Menu");
    InitStartMenuWindow(bootInfo);

    //while (true);
    PrintMsg("> Prepare ACPI");
    PrintMsgStartLayer("ACPI");
    PrepareACPI(bootInfo);
    PrintMsgEndLayer("ACPI");
    PrintDebugTerminal();

    

    // for (int i = 0; i < 20; i++)
    //     GlobalRenderer->Clear(Colors.red);

    PrintMsg("> Enabling FPU");
    enable_fpu();
    
    PrintMsg("> Creating OS Ram Disk");
    PrintMsgStartLayer("OS RAM DISK");
    AddToStack();
    {

        uint64_t totSize = 500000;
        for (int i = 0; i < bootInfo->maabZIP->fileCount; i++)
        {
            totSize += bootInfo->maabZIP->files[i].size;
        }

        totSize += 6000000;
        uint64_t totSize2 = totSize;
        totSize2 /= 512;
        totSize -= 200000;

        DiskInterface::RamDiskInterface* ramDisk = new DiskInterface::RamDiskInterface(totSize2);
        
        osData.diskInterfaces.add(ramDisk);

        DiskInterface::GenericDiskInterface* diskInterface = ramDisk;
        PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)new PartitionInterface::MRAPSPartitionInterface(diskInterface);
        partInterface->InitAndSavePartitionTable();

        partInterface->CreatePartition(partInterface->partitionList[2], totSize);

        partInterface->partitionList[2]->driveName=StrCopy("bruh");
        partInterface->partitionList[2]->driveNameLen=StrLen("bruh");
        partInterface->SavePartitionTable();

        FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)new FilesystemInterface::MrafsFilesystemInterface(partInterface, partInterface->partitionList[2]);
        fsInterface->InitAndSaveFSTable();

        for (int i = 0; i < bootInfo->maabZIP->fileCount; i++)
        {
            PrintMsgColSL("FILE: \"{}\"", bootInfo->maabZIP->files[i].filename, Colors.yellow);
            PrintMsgCol(" ({} bytes)", to_string(bootInfo->maabZIP->files[i].size), Colors.bgreen);
            fsInterface->CreateFile(bootInfo->maabZIP->files[i].filename, bootInfo->maabZIP->files[i].size);
            fsInterface->WriteFile(bootInfo->maabZIP->files[i].filename, bootInfo->maabZIP->files[i].size, bootInfo->maabZIP->files[i].fileData);
        }
        fsInterface->SaveFSTable();

        //while (true);

    }
    RemoveFromStack();
    PrintMsgEndLayer("OS RAM DISK");

    PrintMsgEndLayer("BOOT");

    PrintMsgCol("> Inited Kernel!", Colors.bgreen);
    RemoveFromStack();
    return kernelInfo;
}

