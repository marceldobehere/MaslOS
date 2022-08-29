#include "mrafsFileSystemInterface.h"


namespace FilesystemInterface
{
    MrafsFilesystemInterface::MrafsFilesystemInterface(PartitionInterface::GenericPartitionInterface* partitionInterface, PartitionInterface::PartitionInfo* partitionInfo)
    {
        this->InterfaceType = FilesystemInterfaceType::Mrafs;
        this->partitionInterface = partitionInterface;
        this->partitionInfo = partitionInfo;
    }

    const char* MrafsFilesystemInterface::CreateFile(const char* path)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }
    
    const char* MrafsFilesystemInterface::CreateFolder(const char* path)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    const char* MrafsFilesystemInterface::DeleteFile(const char* path)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }

    const char* MrafsFilesystemInterface::DeleteFolder(const char* path)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    const char* MrafsFilesystemInterface::CopyFile(const char* oldPath, const char* newPath)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }

    const char* MrafsFilesystemInterface::CopyFolder(const char* oldPath, const char* newPath)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    const char* MrafsFilesystemInterface::RenameFile(const char* oldPath, const char* newPath)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }

    const char* MrafsFilesystemInterface::RenameFolder(const char* oldPath, const char* newPath)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    bool MrafsFilesystemInterface::FileExists(const char* path)
    {
        return false;
    }

    bool MrafsFilesystemInterface::FolderExists(const char* path)
    {
        return false;
    }


    FileInfo* MrafsFilesystemInterface::GetFileInfo(const char* path)
    {
        return NULL;
    }

    FolderInfo* MrafsFilesystemInterface::GetFolderInfo(const char* path)
    {
        return NULL;
    }


    const char** MrafsFilesystemInterface::GetFiles(const char* path)
    {
        return NULL;
    }


    const char* MrafsFilesystemInterface::ReadFile(const char* path, void** buffer)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }

    const char* MrafsFilesystemInterface::WriteFile(const char* path, void** buffer)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    const char* MrafsFilesystemInterface::ReadFile(const char* path, uint64_t byteCount, void* buffer)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }

    const char* MrafsFilesystemInterface::WriteFile(const char* path, uint64_t byteCount, void* buffer)
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    const char* MrafsFilesystemInterface::SaveFSTable()
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }

    const char* MrafsFilesystemInterface::LoadFSTable()
    {
        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }
}