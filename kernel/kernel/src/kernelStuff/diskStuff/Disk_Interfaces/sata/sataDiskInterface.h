#pragma once
#include "../generic/genericDiskInterface.h"
#include "../../../../devices/ahci/ahci.h"

namespace DiskInterface
{
    class SataDiskInterface : public GenericDiskInterface
    {
    private:
        AHCI::Port* Port;
        uint32_t SectorCount;

    public:
        SataDiskInterface(AHCI::Port* port);
        bool Read(uint64_t sector, uint32_t sectorCount, void* buffer);
        bool Write(uint64_t sector, uint32_t sectorCount, void* buffer);

        bool ReadBytes(uint64_t address, uint64_t count, void* buffer);
        bool WriteBytes(uint64_t address, uint64_t count, void* buffer);
        uint32_t GetMaxSectorCount();
    };
}