#include "fileDiskInterface.h"
#include "../../../../memory/heap.h"
#include "../../../../paging/PageTableManager.h"
#include "../../Filesystem_Interfaces/generic/genericFileSystemInterface.h"
#include "../../../../cStdLib/cstrTools.h"
#include "../../../../osData/osData.h"

namespace DiskInterface
{
    uint32_t FileDiskInterface::GetMaxSectorCount()
    {
        FilesystemInterface::FileInfo* fileInfo = ((FilesystemInterface::GenericFilesystemInterface*)FsInterface)->GetFileInfo(this->filename);
        if (fileInfo != NULL)
            return fileInfo->sizeInBytes / 512;
        else
            return 0;
    }

    FileDiskInterface::FileDiskInterface(const char* filename, /*FilesystemInterface::GenericFilesystemInterface*/void* fsInterface)
    {
        InterfaceType = DiskInterface::Ram;
        this->filename = StrCopy(filename);
        this->FsInterface = fsInterface;
        this->InterfaceType = DiskInterfaceType::File;
    }

    bool FileDiskInterface::Read(uint64_t sector, uint32_t sectorCount, void* buffer)
    {
        FilesystemInterface::GenericFilesystemInterface* fsInterface = ((FilesystemInterface::GenericFilesystemInterface*)FsInterface);
        FilesystemInterface::FileInfo* fileInfo = fsInterface->GetFileInfo(this->filename);
        if (fileInfo == NULL)
            return false;
        else
            if ((sector + sectorCount)*512 > fileInfo->sizeInBytes)
                return false;
        
        const char* res = fsInterface->ReadFileBuffer(filename, sector * 512, sectorCount * 512, buffer);
        return res == FilesystemInterface::FSCommandResult.SUCCESS;
    }

    bool FileDiskInterface::Write(uint64_t sector, uint32_t sectorCount, void* buffer)
    {
        FilesystemInterface::GenericFilesystemInterface* fsInterface = ((FilesystemInterface::GenericFilesystemInterface*)FsInterface);
        FilesystemInterface::FileInfo* fileInfo = fsInterface->GetFileInfo(this->filename);
        if (fileInfo == NULL)
            return false;
        else
            if ((sector + sectorCount)*512 > fileInfo->sizeInBytes)
                return false;
        
        const char* res = fsInterface->WriteFileBuffer(filename, sector * 512, sectorCount * 512, buffer);
        return res == FilesystemInterface::FSCommandResult.SUCCESS;
    }

    bool FileDiskInterface::ReadBytes(uint64_t address, uint64_t count, void* buffer)
    {
        FilesystemInterface::GenericFilesystemInterface* fsInterface = ((FilesystemInterface::GenericFilesystemInterface*)FsInterface);
        FilesystemInterface::FileInfo* fileInfo = fsInterface->GetFileInfo(this->filename);
        if (fileInfo == NULL)
            return false;
        else
            if ((address + count) > fileInfo->sizeInBytes)
                return false;
        
        const char* res = fsInterface->ReadFileBuffer(filename, address, count, buffer);
        return res == FilesystemInterface::FSCommandResult.SUCCESS;
    }

    bool FileDiskInterface::WriteBytes(uint64_t address, uint64_t count, void* buffer)
    {
        FilesystemInterface::GenericFilesystemInterface* fsInterface = ((FilesystemInterface::GenericFilesystemInterface*)FsInterface);
        FilesystemInterface::FileInfo* fileInfo = fsInterface->GetFileInfo(this->filename);
        if (fileInfo == NULL)
            return false;
        else
            if ((address + count) > fileInfo->sizeInBytes)
                return false;
        
        const char* res = fsInterface->WriteFileBuffer(filename, address, count, buffer);
        return res == FilesystemInterface::FSCommandResult.SUCCESS;
    }
}