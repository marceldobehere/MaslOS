#include "interrupts.h"

__attribute__((interrupt)) void PageFault_handler(struct interrupt_frame* frame)
{
    Panic("Page Fault Detected!");
}

