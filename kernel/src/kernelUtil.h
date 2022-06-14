#pragma once

#include <stddef.h>
#include "BasicRenderer.h"
#include "Cols.h"
#include "cstr.h" 


#include "efiMemory.h"
#include "memory.h" 

//#include "userinput/mouse.h"

#include "paging/PageFrameAllocator.h"  
#include "paging/PageMapIndexer.h"
#include "paging/paging.h" 
#include "paging/PageTableManager.h"


extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

struct KernelInfo
{
    PageTableManager* pageTableManager;
};

#include "OSDATA/osdata.h"
#include "OSDATA/userdata.h"
#include "OSDATA/MStack/MStackM.h"

#include "gdt/gdt.h" 


#include "interrupts/IDT.h"
#include "interrupts/interrupts.h"

#include "panic.h"   
#include "IO.h"

#include "userinput/keyboard.h"
#include "userinput/mouse.h"
 
#include "memory/heap.h"
#include "cmdParsing/cmdParser.h"

#include "scheduling-pit/pit.h"

#include "customClasses/array.h"
#include "customClasses/list.h"


//extern KernelInfo kernelInfo;
//extern PageTableManager pageTableManager;
//extern PageFrameAllocator t;
void PrepareMemory(BootInfo* bootInfo);

//extern IDTR idtr;

void SetIDTGate(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector);

void PrepareInterrupts();


//extern BasicRenderer r;
KernelInfo InitializeKernel(BootInfo* bootInfo);