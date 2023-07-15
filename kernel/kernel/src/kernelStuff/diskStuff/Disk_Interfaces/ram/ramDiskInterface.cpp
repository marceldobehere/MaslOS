#include "ramDiskInterface.h"
#include "../../../../memory/heap.h"
#include "../../../../paging/PageTableManager.h"

namespace DiskInterface
{
    uint32_t RamDiskInterface::GetMaxSectorCount()
    {
        return SectorCount;
    }

    RamDiskInterface::RamDiskInterface(uint64_t sectorCount)
    {
        InterfaceType = DiskInterface::Ram;
        SectorCount = sectorCount;
        Buffer = (uint8_t*)_Malloc(sectorCount * 512, "Malloc For Ram Disk");
        _memset(Buffer, 0, sectorCount * 512);
    }

    bool RamDiskInterface::Read(uint64_t sector, uint32_t sectorCount, void* buffer)
    {
        uint8_t* buf = (uint8_t*)buffer;
        for (uint64_t s = sector; s < sector + sectorCount; s++)
        {
            if (s >= SectorCount)
                return false;
            _memcpy(Buffer + (s * 512), buf + (s * 512), 512);
        }
        return true;
    }

    bool RamDiskInterface::Write(uint64_t sector, uint32_t sectorCount, void* buffer)
    {
        uint8_t* buf = (uint8_t*)buffer;
        for (uint64_t s = sector; s < sector + sectorCount; s++)
        {
            if (s >= SectorCount)
                return false;
            _memcpy(buf + (s * 512), Buffer + (s * 512), 512);
        }
        return true;
    }

    bool RamDiskInterface::ReadBytes(uint64_t address, uint64_t count, void* buffer)
    {
        if (address + count > SectorCount * 512)
            return false;

        for (uint64_t i = 0; i < count; i++)
            ((uint8_t*)buffer)[i] = Buffer[address + i];
        
        return true;
    }

    bool RamDiskInterface::WriteBytes(uint64_t address, uint64_t count, void* buffer)
    {
        if (address + count > SectorCount * 512)
            return false;

        for (uint64_t i = 0; i < count; i++)
            Buffer[address + i] = ((uint8_t*)buffer)[i];

        return true;
    }
}