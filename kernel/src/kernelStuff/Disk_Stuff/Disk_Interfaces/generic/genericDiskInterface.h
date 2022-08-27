#pragma once
#include <stdint.h>
//#include "../../../../memory/heap.h"

namespace DiskInterface
{
    enum DiskInterfaceType
    {
        Generic = 0,
        Ram = 1,
        Sata = 2
    };

    class GenericDiskInterface
    {
    private:

    public:
        DiskInterfaceType InterfaceType = DiskInterface::Generic;
        //virtual ~GenericDiskInterface() {}
        bool Read(uint64_t sector, uint32_t sectorCount, void* buffer);
        bool Write(uint64_t sector, uint32_t sectorCount, void* buffer);
        uint32_t GetMaxSectorCount();
    };
}