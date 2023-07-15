#pragma once


#include <stddef.h>
#include <stdint.h>

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;


#include "../../rendering/BasicRenderer.h"
#include "../../rendering/Cols.h"
#include "cstr.h" 

// #include "paging/PageFrameAllocator.h"  
// #include "paging/PageMapIndexer.h"
// #include "paging/paging.h" 
#include "../../paging/PageTableManager.h"




struct KernelInfo
{
    PageTableManager* pageTableManager;
};


#include "stackmacro.h"


//#include "../memory/efiMemory.h"
#include "../memory/memory.h" 

//#include "userinput/mouse.h"



#include "../../osData/osData.h"
#include "../../osData/userData.h"
#include "../../osData/MStack/MStackM.h"

#include "../other_IO/gdt/gdt.h" 


#include "../../interrupts/IDT.h"
#include "../../interrupts/interrupts.h"

#include "../../interrupts/panic.h"   
#include "../IO/IO.h"

#include "../../userinput/keyboard.h"
#include "../../userinput/mouse.h"
 
#include "../../memory/heap.h"
#include "../../cmdParsing/cmdParser.h"

#include "../other_IO/pit/pit.h"

//#include "../../customClasses/array.h"
#include "../../customClasses/list.h"

#include "../kernelFiles/kernelFiles.h"
#include "../kernelFiles/ZIPStruct.h"

#include "../other_IO/acpi/acpi.h"
#include "../other_IO/pci/pci.h"

#include "../../rendering/VirtualRenderer.h"

#include "../../WindowStuff/Taskbar/taskbar.h"
#include "../../WindowStuff/SubInstances/debugTerminalInstance/debugTerminalInstance.h"
#include "../../WindowStuff/SubInstances/newTerminalInstance/newTerminalInstance.h"

//#include "../other_IO/sb16/sb16.h"

void PrepareACPI(BootInfo* bootInfo);

//extern KernelInfo kernelInfo;
//extern PageTableManager pageTableManager;
//extern PageFrameAllocator t;
void PrepareMemory(BootInfo* bootInfo);

//extern IDTR idtr;

void SetIDTGate(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector);

void PrepareInterrupts();

void PrepareTerminals();

//extern BasicRenderer r;
KernelInfo InitializeKernel(BootInfo* bootInfo);