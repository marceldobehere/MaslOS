#include "fileSystemStructs.h"
#include "../../../../cmdParsing/cstrTools.h"
#include "../../../../memory/heap.h"

namespace FilesystemInterface
{
    const char* FilesystemInterfaceTypeStr[3] 
    {
        "None",
        "Generic",
        "MRAFS"
    };

    _FSCommandResult FSCommandResult;

    BaseInfo::BaseInfo(const char* path, bool writeProtected, bool hidden, bool systemFile)
    {
        this->path = StrCopy(path);
        this->pathLen = StrLen(this->path);
        this->writeProtected = writeProtected;
        this->hidden = hidden;
        this->systemFile = systemFile;
    }

    // BaseInfo::BaseInfo()
    // {
    //     path = StrCopy("");
    //     pathLen = StrLen(path);
    //     writeProtected = false;
    //     hidden = false;
    //     systemFile = false;
    // }
    void BaseInfo::Destroy()
    {
        pathLen = 0;
        _Free((void*)path);
        path = 0;
        _Free(this);
    }

    FolderInfo::FolderInfo(BaseInfo* baseInfo)
    {
        this->baseInfo = baseInfo;
    }
    void FolderInfo::Destroy()
    {
        this->baseInfo->Destroy();
        _Free(this);
    }

    FileInfo::FileInfo(BaseInfo* baseInfo, uint64_t sizeInBytes, uint64_t locationInBytes)
    {
        this->baseInfo = baseInfo;
        this->sizeInBytes = sizeInBytes;
        this->locationInBytes = locationInBytes;
    }
    void FileInfo::Destroy()
    {
        this->baseInfo->Destroy();
        _Free(this);
    }
}
