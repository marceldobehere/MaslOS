#include "genericDiskInterface.h"
#include "../../../../OSDATA/osdata.h"
#include "../ram/ramDiskInterface.h"
#include "../sata/sataDiskInterface.h"

namespace DiskInterface
{
    bool GenericDiskInterface::Read(uint64_t sector, uint32_t sectorCount, void* buffer)
    {
        switch (InterfaceType)
        {
            case DiskInterface::Ram:
            {
                return ((RamDiskInterface*)this)->Read(sector, sectorCount, buffer);
            }
            case DiskInterface::Sata:
            {
                return ((SataDiskInterface*)this)->Read(sector, sectorCount, buffer);
            }
        }
        //osData.mainTerminalWindow->Log("DISK IS GENERIC!!!");
        return false;
    }

    bool GenericDiskInterface::Write(uint64_t sector, uint32_t sectorCount, void* buffer)
    {
        switch (InterfaceType)
        {
            case DiskInterface::Ram:
            {
                return ((RamDiskInterface*)this)->Write(sector, sectorCount, buffer);
            }
            case DiskInterface::Sata:
            {
                return ((SataDiskInterface*)this)->Write(sector, sectorCount, buffer);
            }
        }
        //osData.mainTerminalWindow->Log("DISK IS GENERIC!!!");
        return false;
    }
}