#include "interrupts.h"

#include "../Rendering/BasicRenderer.h"
#include "../userinput/keyboard.h"
#include "../userinput/mouse.h"
#include "panic.h"
#include "../kernelStuff/IO/IO.h"
#include "../kernelStuff/other_IO/pit/pit.h"
#include "../OSDATA/MStack/MStackM.h"

__attribute__((interrupt)) void PageFault_handler(struct interrupt_frame* frame)
{
    AddToStack();
    Panic("Page Fault Detected!", false);
    RemoveFromStack();
}

__attribute__((interrupt)) void DoubleFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Double Fault Detected!");
    RemoveFromStack();
}

__attribute__((interrupt)) void GPFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Protection Fault Detected!");
    //Panic("General Protection Fault Detected! {}", to_string(*((uint64_t*)frame)), true);
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


void RemapPIC() 
{
    AddToStack();
    uint8_t a1, a2;
    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, 0xa1);
    io_wait();
    outb(PIC2_DATA, 0xa2);
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



