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

    uint32_t GenericDiskInterface::GetMaxSectorCount()
    {
        switch (InterfaceType)
        {
            case DiskInterface::Ram:
            {
                return ((RamDiskInterface*)this)->GetMaxSectorCount();
            }
            case DiskInterface::Sata:
            {
                return ((SataDiskInterface*)this)->GetMaxSectorCount();
            }
        }
        //osData.mainTerminalWindow->Log("DISK IS GENERIC!!!");
        return 0;
    }


    bool GenericDiskInterface::ReadBytes(uint64_t address, uint64_t count, void* buffer)
    {
        switch (InterfaceType)
        {
            case DiskInterface::Ram:
            {
                return ((RamDiskInterface*)this)->ReadBytes(address, count, buffer);
            }
            case DiskInterface::Sata:
            {
                return ((SataDiskInterface*)this)->ReadBytes(address, count, buffer);
            }
        }
        //osData.mainTerminalWindow->Log("DISK IS GENERIC!!!");
        return false;
    }
    
    
    bool GenericDiskInterface::WriteBytes(uint64_t address, uint64_t count, void* buffer)
    {
        switch (InterfaceType)
        {
            case DiskInterface::Ram:
            {
                return ((RamDiskInterface*)this)->WriteBytes(address, count, buffer);
            }
            case DiskInterface::Sata:
            {
                return ((SataDiskInterface*)this)->WriteBytes(address, count, buffer);
            }
        }
        //osData.mainTerminalWindow->Log("DISK IS GENERIC!!!");
        return false;
    }
}