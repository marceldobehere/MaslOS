#pragma once
#include "paging.h"
#include "PageMapIndexer.h"
#include "PageFrameAllocator.h"
#include "../memory.h"
#include <stdint.h>


class PageTableManager
{
    public:
    PageTable* PML4;
    PageTableManager(PageTable* PML4Address);

    void MapMemory(void* virtualMemory, void* physicalMemory);

};

extern PageTableManager GlobalPageTableManager;