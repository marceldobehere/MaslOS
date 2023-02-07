#include "interrupts.h"

#include "../Rendering/BasicRenderer.h"
#include "../userinput/keyboard.h"
#include "../userinput/mouse.h"
#include "panic.h"
#include "../kernelStuff/IO/IO.h"
#include "../kernelStuff/other_IO/pit/pit.h"
#include "../OSDATA/MStack/MStackM.h"
#include "../OSDATA/osStats.h"

#define SURVIVE_CRASH     void** p = search((void**)&p, __builtin_return_address(0)); \
    *p = (void*)BruhusSafus;


#include "../kernel.h"

extern "C" void BruhusSafus()
{
    //GlobalRenderer->Clear(Colors.green);
    //PIT::Sleep(1000);
    // GlobalRenderer->Clear(Colors.red);
    // while (true);
    RecoverDed();
}

void** search(void** addr, void* value) __attribute__((noinline));
void** search(void** addr, void* value)
{
    while(*addr != value) addr++;
    return addr;
}
// https://stackoverflow.com/questions/27213382/how-to-modify-return-address-on-stack-in-c-or-assembly








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

    SURVIVE_CRASH
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
        AddToKeyboardList(scancode);
        //HandleKeyboard(scancode);  
    PIC_EndMaster();
    RemoveFromStack();
}

__attribute__((interrupt)) void MouseInt_handler(interrupt_frame* frame)
{ 
    AddToStack();
    osStats.lastMouseCall = PIT::TimeSinceBootMS();
    uint8_t mousedata = inb(0x60);
    

    HandlePS2Mouse(mousedata);

    PIC_EndSlave();
    RemoveFromStack();
}

__attribute__((interrupt)) void PITInt_handler(interrupt_frame* frame)
{ 
    AddToStack();
    PIT::Tick();
    PIC_EndMaster();
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
    AddToStack();
    outb(PIC1_COMMAND, PIC_EOI);
    io_wait();
    RemoveFromStack();
}


void PIC_EndSlave()
{
    AddToStack();
    outb(PIC2_COMMAND, PIC_EOI);
    io_wait();
    outb(PIC1_COMMAND, PIC_EOI);
    io_wait();
    RemoveFromStack();
}



