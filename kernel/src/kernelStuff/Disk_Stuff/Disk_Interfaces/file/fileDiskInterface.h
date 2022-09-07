#pragma once
#include "../generic/genericDiskInterface.h"

namespace DiskInterface
{
    class FileDiskInterface : public GenericDiskInterface
    {
    private:
        const char* filename;
        //uint32_t SectorCount;
        void* FsInterface;

    public:
        FileDiskInterface(const char* filename, void* fsInterface);
        bool Read(uint64_t sector, uint32_t sectorCount, void* buffer);
        bool Write(uint64_t sector, uint32_t sectorCount, void* buffer);  

        bool ReadBytes(uint64_t address, uint64_t count, void* buffer);
        bool WriteBytes(uint64_t address, uint64_t count, void* buffer);
        uint32_t GetMaxSectorCount();
    };
}