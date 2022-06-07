#include "interrupts.h"

#include "../BasicRenderer.h"
#include "../userinput/keyboard.h"
#include "../userinput/mouse.h"
#include "../panic.h"
#include "../IO.h"

__attribute__((interrupt)) void PageFault_handler(struct interrupt_frame* frame)
{
    Panic("Page Fault Detected!", false);
    uint64_t Register = 0;
    //Panic("Page Fault Detected! {}", to_string(*((uint64_t*)frame)), true);
    GlobalRenderer->Println();
	GlobalRenderer->Println("Register dump: ");
    GlobalRenderer->Println();
	GlobalRenderer->Print("rax: ");
	asm volatile("mov %%rax, %0" : "=r"(Register));
	GlobalRenderer->Print("0x");
	GlobalRenderer->Print(ConvertHexToString(Register));
	GlobalRenderer->Print("  ");
	GlobalRenderer->Print("rcx: ");
	asm volatile("mov %%rcx, %0" : "=r"(Register));
	GlobalRenderer->Print("0x");
	GlobalRenderer->Print(ConvertHexToString(Register));
	GlobalRenderer->Print("  ");
	GlobalRenderer->Print("rdx: ");
	asm volatile("mov %%rdx, %0" : "=r"(Register));
	GlobalRenderer->Print("0x");
	GlobalRenderer->Print(ConvertHexToString(Register));
	GlobalRenderer->Print("  ");
	GlobalRenderer->Print("rbx: ");
	asm volatile("mov %%rbx, %0" : "=r"(Register));
	GlobalRenderer->Print("0x");
	GlobalRenderer->Print(ConvertHexToString(Register));
	GlobalRenderer->Print("  ");
    GlobalRenderer->Println();
	GlobalRenderer->Print("rsp: ");
	asm volatile("mov %%rsp, %0" : "=r"(Register));
	GlobalRenderer->Print("0x");
	GlobalRenderer->Print(ConvertHexToString(Register));
	GlobalRenderer->Print("  ");
	GlobalRenderer->Print("rbp: ");
	asm volatile("mov %%rbp, %0" : "=r"(Register));
	GlobalRenderer->Print("0x");
	GlobalRenderer->Print(ConvertHexToString(Register));
	GlobalRenderer->Print("  ");
	GlobalRenderer->Print("rsi: ");
	asm volatile("mov %%rsi, %0" : "=r"(Register));
	GlobalRenderer->Print("0x");
	GlobalRenderer->Print(ConvertHexToString(Register));
	GlobalRenderer->Print("  ");
	GlobalRenderer->Print("rdi: ");
	asm volatile("mov %%rdi, %0" : "=r"(Register));
	GlobalRenderer->Print("0x");
	GlobalRenderer->Print(ConvertHexToString(Register));
    GlobalRenderer->Println();
	asm("hlt");
    while(true);
}

__attribute__((interrupt)) void DoubleFault_handler(interrupt_frame* frame)
{
    Panic("Double Fault Detected!");
}

__attribute__((interrupt)) void GPFault_handler(interrupt_frame* frame)
{
    Panic("General Protection Fault Detected!");
    //Panic("General Protection Fault Detected! {}", to_string(*((uint64_t*)frame)), true);
}
 
__attribute__((interrupt)) void KeyboardInt_handler(interrupt_frame* frame)
{ 
    uint8_t scancode = inb(0x60);
    //GlobalRenderer->Println("Pressed: {}", to_string((uint64_t)scancode));
    HandleKeyboard(scancode);  
    PIC_EndMaster();

}

__attribute__((interrupt)) void MouseInt_handler(interrupt_frame* frame)
{ 
    uint8_t mousedata = inb(0x60);

    HandlePS2Mouse(mousedata);

    PIC_EndSlave();
}


void RemapPIC() 
{
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

}

void PIC_EndMaster()
{
    outb(PIC1_COMMAND, PIC_EOI);
}


void PIC_EndSlave()
{
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}



