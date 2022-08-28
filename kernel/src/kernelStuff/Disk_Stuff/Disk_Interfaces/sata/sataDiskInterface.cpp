#include "sataDiskInterface.h"
#include "../../../../paging/PageTableManager.h"

namespace DiskInterface
{
    uint32_t SataDiskInterface::GetMaxSectorCount()
    {
        return Port->GetMaxSectorCount();
    }

    SataDiskInterface::SataDiskInterface(AHCI::Port* port)
    {
        InterfaceType = DiskInterface::Sata;
        this->Port = port;
        this->Port->buffer = (uint8_t*)GlobalAllocator->RequestPage(); // 4096 Bytes
    }

    bool SataDiskInterface::Read(uint64_t sector, uint32_t sectorCount, void* buffer)
    {
        uint8_t* buf = (uint8_t*)buffer;
        int sectorCountDiv8 = ((sectorCount - 1) / 8) + 1;
        for (int sect = 0; sect < sectorCountDiv8 - 1; sect++)
        {
            _memset(Port->buffer, 0, 0x1000);
            if (!Port->Read(sector, 8, Port->buffer))
                return false;
            _memcpy(Port->buffer, buf, 0x1000);
            buf += 0x1000;
            sector += 8;
        }

        _memset(Port->buffer, 0, ((sectorCount % 8) << 9));
        if (!Port->Read(sector, sectorCount % 8, Port->buffer))
            return false;
        _memcpy(Port->buffer, buf, ((sectorCount % 8) << 9));

        return true;
    }

    bool SataDiskInterface::Write(uint64_t sector, uint32_t sectorCount, void* buffer)
    {
        uint8_t* buf = (uint8_t*)buffer;
        int sectorCountDiv8 = ((sectorCount - 1) / 8) + 1;
        for (int sect = 0; sect < sectorCountDiv8 - 1; sect++)
        {
            _memset(Port->buffer, 0, 0x1000);
            _memcpy(buf, Port->buffer, 0x1000);
            if (!Port->Write(sector, 8, Port->buffer))
                return false;
            buf += 0x1000;
            sector += 8;
        }

        _memset(Port->buffer, 0, ((sectorCount % 8) << 9));
        _memcpy(buf, Port->buffer, ((sectorCount % 8) << 9));
        if (!Port->Write(sector, sectorCount % 8, Port->buffer))
            return false;
        
        return true;
    }
}