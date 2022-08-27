#pragma once
#include "../generic/genericDiskInterface.h"

namespace DiskInterface
{
    class RamDiskInterface : public GenericDiskInterface
    {
    private:
        uint8_t* Buffer;
        uint32_t SectorCount;

    public:
        RamDiskInterface(uint64_t sectorCount);
        bool Read(uint64_t sector, uint32_t sectorCount, void* buffer);
        bool Write(uint64_t sector, uint32_t sectorCount, void* buffer);  
    };
}