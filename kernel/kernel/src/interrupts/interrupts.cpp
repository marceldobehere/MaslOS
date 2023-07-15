#include "interrupts.h"

#include "../rendering/BasicRenderer.h"
#include "../userinput/keyboard.h"
#include "../userinput/mouse.h"
#include "panic.h"
#include "../kernelStuff/IO/IO.h"
#include "../kernelStuff/other_IO/pit/pit.h"
#include "../osData/MStack/MStackM.h"
#include "../osData/osStats.h"

#include "../kernelStuff/other_IO/acpi/acpiShutdown.h"



#include "../kernel.h"



extern "C" void BruhusSafus()
{

    //GlobalRenderer->Clear(Colors.green);
    //PIT::Sleep(500);
    //GlobalRenderer->Clear(Colors.red);
    //PIT::Sleep(500);
    RecoverDed();

    for (int i = 0; i < 50; i++)
        GlobalRenderer->ClearDotted(Colors.black);    
    for (int i = 0; i < 50; i++)
        GlobalRenderer->Clear(Colors.black);
    GlobalRenderer->color = Colors.white;
    GlobalRenderer->Println("Shutting down...");
    for (int i = 0; i < 50; i++)
        GlobalRenderer->ClearButDont();    

    PowerOffAcpi();


    GlobalRenderer->Clear(Colors.black);
    GlobalRenderer->Println("The ACPI shutdown failed!", Colors.yellow);

    GlobalRenderer->Println();
    GlobalRenderer->Println("Please shut down the computer manually.", Colors.white);

    while (true)
        asm("hlt");
}

void DoSafe()
{
    BruhusSafus();
}

//void** search(void** addr, void* value) __attribute__((noinline));
void** search(void** addr, void* value)
{
    while(*addr != value) addr++;
    return addr;
}
// https://stackoverflow.com/questions/27213382/how-to-modify-return-address-on-stack-in-c-or-assembly


#define SURVIVE_CRASH_OLD     void** p = search((void**)&p, __builtin_return_address(0)); \
    *p = (void*)BruhusSafus;

#define SURVIVE_CRASH asm volatile("mov %0 ,8(%%rbp)" : : "r"((void*)BruhusSafus) );

//asm volatile("mov 8(%%rbp),%0" : "=r"(returnaddr) : : );
//asm volatile("mov %%rax, %0" : "=r"(Register));
//asm volatile("mov %0, %%cr3" : : "r" (PML4) );
//asm volatile("mov %0, 8(%%rbp)" : "r"((void*)BruhusSafus) : : );


__attribute__((interrupt)) void GenericInt_handler(interrupt_frame* frame)
{
    AddToStack();
    //Panic("GENERIC INTERRUPT BRUH", true);   
    RemoveFromStack();
}


__attribute__((interrupt)) void PageFault_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("Page Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void DoubleFault_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("Double Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void GPFault_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();

    HeapCheck(false);

    Panic("General Protection Fault Detected! (ERROR: {})", to_string(frame->base_frame.error_code), false);
    //Panic("General Protection Fault Detected! {}", to_string(*((uint64_t*)frame)), true);
    RemoveFromStack();

    osData.NO_INTERRUPTS = true;
    BruhusSafus();


    //SURVIVE_CRASH
}   

__attribute__((interrupt)) void GenFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void GenFault1_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Fault 1 Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void GenFault2_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Fault 2 Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void GenFaultWithError_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("General Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}


__attribute__((interrupt)) void GenFloatFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Float Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}



__attribute__((interrupt)) void GenMathFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Math Fault Detected!", false);
    RemoveFromStack();



    SURVIVE_CRASH
}

__attribute__((interrupt)) void Debug_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Debug thing Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void Breakpoint_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Breakpoint Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}
 
__attribute__((interrupt)) void KeyboardInt_handler(interrupt_frame* frame)
{ 
    AddToStack();
    osStats.lastKeyboardCall = PIT::TimeSinceBootMS();
    uint8_t scancode = inb(0x60);
    //GlobalRenderer->Println("Pressed: {}", to_string((uint64_t)scancode));
    if (osData.booting)
        osData.booting = false;
    else
        AddScancodeToKeyboardList(scancode);
        //HandleKeyboard(scancode);  
    PIC_EndMaster();
    RemoveFromStack();
}

#define MOUSE_PORT   0x60
#define MOUSE_STATUS 0x64
#define MOUSE_ABIT   0x02
#define MOUSE_BBIT   0x01
#define MOUSE_WRITE  0xD4
#define MOUSE_F_BIT  0x20
#define MOUSE_V_BIT  0x08
// https://github.com/stevej/osdev/blob/master/kernel/devices/mouse.c


__attribute__((interrupt)) void MouseInt_handler(interrupt_frame* frame)
{ 
    AddToStack();
    //Panic("GENERIC INTERRUPT BRUH", true);   
    //osStats.lastMouseCall = PIT::TimeSinceBootMS();
    //io_wait();
    //Mousewait();

	uint8_t status = inb(MOUSE_STATUS);
	while (status & MOUSE_BBIT) 
    {
        int8_t mouse_in = inb(MOUSE_PORT);
		if (status & MOUSE_F_BIT)
        {
            HandlePS2Mouse(mouse_in);
        }
        status = inb(MOUSE_STATUS);
    }


    //uint8_t mousedata = inb(0x60);
    //PIC_EndSlave();
    Mousewait();
    //io_wait();
    
    //HandlePS2Mouse(mousedata);

    PIC_EndSlave();
    //PIC_EndMaster();
    RemoveFromStack();
}

bool speakA = false;

#include "../audio/audioDevStuff.h"

__attribute__((interrupt)) void PITInt_handler(interrupt_frame* frame)
{ 
    AddToStack();
    PIT::Tick();
    PIC_EndMaster();
    AudioDeviceStuff::play(PIT::FreqAdder);

    // TestSetSpeakerPosition(speakA);
    // speakA = !speakA;
    RemoveFromStack();
}

__attribute__((interrupt)) void VirtualizationFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Virtualization Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void ControlProtectionFault_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("Control Protection Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void HypervisorFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Hypervisor Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void VMMCommunicationFault_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("VMM Communication Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void SecurityException_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("Security Exception Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}


__attribute__((interrupt)) void InvalidOpCode_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("Invalid Opcode Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}














void TestSetSpeakerPosition(bool in)
{
    uint8_t t = inb(0x61);
    //io_wait();

    if (in)
        t |= 3;
    else
        t &= 0xFC;
    
    outb(0x61, t);
    //io_wait();
}


void RemapPIC(uint8_t _a1, uint8_t _a2) 
{
    AddToStack();
    // uint8_t a1, a2;
    
    // a1 = inb(PIC1_DATA);
    // //io_wait();
    // a2 = inb(PIC2_DATA);
    // //io_wait();

    AddToStack();
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    RemoveFromStack();

    AddToStack();
    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();
    RemoveFromStack();

    AddToStack();
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();
    RemoveFromStack();

    AddToStack();
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();
    RemoveFromStack();

    // AddToStack();
    // outb(PIC1_DATA, 0xa1);
    // io_wait();
    // outb(PIC2_DATA, 0xa2);
    // io_wait();
    // RemoveFromStack();
    
    // restore masks
    // outb(PIC1_DATA, a1);
    // outb(PIC2_DATA, a2);  
    io_wait();
    outb(PIC1_DATA, _a1);
    io_wait();
    outb(PIC2_DATA, _a2);  
    io_wait();

    io_wait();
    outb(PIC1_DATA, _a1);
    io_wait();
    outb(PIC2_DATA, _a2);  
    io_wait();

    RemoveFromStack();
}

void PIC_EndMaster()
{
    //AddToStack();
    io_wait();
    outb(PIC1_COMMAND, PIC_EOI);
    io_wait();
    //RemoveFromStack();
}


void PIC_EndSlave()
{
    AddToStack();
    io_wait();
    outb(PIC1_COMMAND, PIC_EOI);
    io_wait();
    outb(PIC2_COMMAND, PIC_EOI);
    io_wait();
    RemoveFromStack();
}


#define IRQHandlerCode(irq) \
    {AddToStack(); \
    IRQGenericDriverHandler(irq, frame); \
    RemoveFromStack(); }\


__attribute__((interrupt)) void IRQ2_handler(interrupt_frame* frame)
    IRQHandlerCode(2); //Cascade
__attribute__((interrupt)) void IRQ3_handler(interrupt_frame* frame)
    IRQHandlerCode(3); //COM2
__attribute__((interrupt)) void IRQ4_handler(interrupt_frame* frame)
    IRQHandlerCode(4); //COM1
__attribute__((interrupt)) void IRQ5_handler(interrupt_frame* frame)
    IRQHandlerCode(5); //LPT2
__attribute__((interrupt)) void IRQ6_handler(interrupt_frame* frame)
    IRQHandlerCode(6); //Floppy
__attribute__((interrupt)) void IRQ7_handler(interrupt_frame* frame)
    IRQHandlerCode(7); //LPT1
__attribute__((interrupt)) void IRQ8_handler(interrupt_frame* frame)
    IRQHandlerCode(8); //RTC
__attribute__((interrupt)) void IRQ9_handler(interrupt_frame* frame)
    IRQHandlerCode(9); //Free
__attribute__((interrupt)) void IRQ10_handler(interrupt_frame* frame)
    IRQHandlerCode(10); //Free
__attribute__((interrupt)) void IRQ11_handler(interrupt_frame* frame)
    IRQHandlerCode(11); //Free
__attribute__((interrupt)) void IRQ14_handler(interrupt_frame* frame)
    IRQHandlerCode(14); //Primary ATA
__attribute__((interrupt)) void IRQ15_handler(interrupt_frame* frame)
    IRQHandlerCode(15); //Secondary ATA

void* IRQHandlerCallbackHelpers[256];
void* IRQHandlerCallbackFuncs[256];

// void (*IRQHandlerCallbacks[256]) (interrupt_frame*) = {
//     NULL, //PIT
//     NULL, //Keyboard
//     NULL, //Cascade
//     NULL, //COM2
//     NULL, //COM1
//     NULL, //LPT2
//     NULL, //Floppy
//     NULL, //LPT1
//     NULL, //RTC
//     NULL, //Free
//     NULL, //Free
//     NULL, //Free
//     NULL, //PS2 Mouse
//     NULL, //FPU
//     NULL, //Primary ATA
//     NULL, //Secondary ATA
// };

void IRQGenericDriverHandler(int irq, interrupt_frame* frame)
{
    //Panic("BRUH IRQ {}", to_string(irq), true);
    if (irq < 0 || irq > 15)
    {
        Panic("Invalid IRQ {}", to_string(irq), true);
    }

    void* callbackFunc = IRQHandlerCallbackFuncs[irq];
    if (callbackFunc != NULL)
    {
        void* classInstance = IRQHandlerCallbackHelpers[irq];
        if (classInstance == NULL)
        {
            // Static Func
            ((void (*)(interrupt_frame*))callbackFunc)(frame);
        }
        else
        {
            // Member Func
            ((void (*)(void*, interrupt_frame*))callbackFunc)(classInstance, frame);
        }
    }
    else
        ;//Panic("Unhandled IRQ {}", to_string(irq), false);

    if (irq >= 8)
        PIC_EndSlave();
    else
        PIC_EndMaster();
}