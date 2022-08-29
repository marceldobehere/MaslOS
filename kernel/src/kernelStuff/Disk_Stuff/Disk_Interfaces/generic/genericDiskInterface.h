#pragma once
#include <stdint.h>
#include <stddef.h>
//#include "../../../../memory/heap.h"
//#include "../../Partition_Interfaces/generic/genericPartitionInterface.h"


namespace DiskInterface
{
    enum DiskInterfaceType
    {
        Generic = 0,
        Ram = 1,
        Sata = 2
    };

    extern const char* DiskInterfaceTypeStr[];

    class GenericDiskInterface
    {
    private:

    public:
        DiskInterfaceType InterfaceType = DiskInterface::Generic;
        void* partitionInterface = NULL;
        //virtual ~GenericDiskInterface() {}
        bool Read(uint64_t sector, uint32_t sectorCount, void* buffer);
        bool Write(uint64_t sector, uint32_t sectorCount, void* buffer);

        bool ReadBytes(uint64_t address, uint64_t count, void* buffer);
        bool WriteBytes(uint64_t address, uint64_t count, void* buffer);
        uint32_t GetMaxSectorCount();
    };
}