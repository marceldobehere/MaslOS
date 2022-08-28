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

        bool ReadBytes(uint64_t address, uint64_t count, void* buffer);
        bool WriteBytes(uint64_t address, uint64_t count, void* buffer);
        uint32_t GetMaxSectorCount();
    };
}