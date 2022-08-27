#pragma once
#include "../generic/genericDiskInterface.h"
#include "../../../other_IO/ahci/ahci.h"

namespace DiskInterface
{
    class SataDiskInterface : public GenericDiskInterface
    {
    private:
        AHCI::Port* Port;

    public:
        SataDiskInterface(AHCI::Port* port);
        bool Read(uint64_t sector, uint32_t sectorCount, void* buffer);
        bool Write(uint64_t sector, uint32_t sectorCount, void* buffer);
        
    };
}