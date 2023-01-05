#include "interrupts.h"

#include "../Rendering/BasicRenderer.h"
#include "../userinput/keyboard.h"
#include "../userinput/mouse.h"
#include "panic.h"
#include "../kernelStuff/IO/IO.h"
#include "../kernelStuff/other_IO/pit/pit.h"
#include "../OSDATA/MStack/MStackM.h"

__attribute__((interrupt)) void PageFault_handler(struct interrupt_frame* frame, uint64_t error)
{
    AddToStack();
    Panic("Page Fault Detected!");
    RemoveFromStack();
}

__attribute__((interrupt)) void DoubleFault_handler(interrupt_frame* frame, uint64_t error)
{
    AddToStack();
    Panic("Double Fault Detected!");
    RemoveFromStack();
}

__attribute__((interrupt)) void GPFault_handler(interrupt_frame* frame, uint64_t error)
{
    AddToStack();
    Panic("General Protection Fault Detected!!");
    //Panic("General Protection Fault Detected! {}", to_string(*((uint64_t*)frame)), true);
    RemoveFromStack();
}

__attribute__((interrupt)) void GenFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Fault Detected!", true);
    RemoveFromStack();
}

__attribute__((interrupt)) void GenFaultWithError_handler(interrupt_frame* frame, uint64_t error)
{
    AddToStack();
    Panic("General Fault Detected!", true);
    RemoveFromStack();
}


__attribute__((interrupt)) void GenFloatFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Float Fault Detected!");
    RemoveFromStack();
}

__attribute__((interrupt)) void GenMathFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Math Fault Detected!");
    RemoveFromStack();
}

__attribute__((interrupt)) void Debug_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Debug thing Detected!");
    RemoveFromStack();
}

__attribute__((interrupt)) void Breakpoint_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Breakpoint Detected!", true);
    RemoveFromStack();
}
 
__attribute__((interrupt)) void KeyboardInt_handler(interrupt_frame* frame)
{ 
    AddToStack();
    uint8_t scancode = inb(0x60);
    //GlobalRenderer->Println("Pressed: {}", to_string((uint64_t)scancode));
    if (osData.booting)
        osData.booting = false;
    else
        HandleKeyboard(scancode);  
    PIC_EndMaster();
    RemoveFromStack();
}

__attribute__((interrupt)) void MouseInt_handler(interrupt_frame* frame)
{ 
    AddToStack();
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
    Panic("Virtualization Fault Detected!", true);
    RemoveFromStack();
}

__attribute__((interrupt)) void ControlProtectionFault_handler(interrupt_frame* frame, uint64_t error)
{
    AddToStack();
    Panic("Control Protection Fault Detected!", true);
    RemoveFromStack();
}

__attribute__((interrupt)) void HypervisorFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Hypervisor Fault Detected!", true);
    RemoveFromStack();
}

__attribute__((interrupt)) void VMMCommunicationFault_handler(interrupt_frame* frame, uint64_t error)
{
    AddToStack();
    Panic("VMM Communication Fault Detected!", true);
    RemoveFromStack();
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

    RemoveFromStack();
}

void PIC_EndMaster()
{
    AddToStack();
    outb(PIC1_COMMAND, PIC_EOI);
    RemoveFromStack();
}


void PIC_EndSlave()
{
    AddToStack();
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
    RemoveFromStack();
}



