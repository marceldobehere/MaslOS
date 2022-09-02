#pragma once
#include "fileSystemStructs.h"
//#include "../../../../memory/heap.h"
//#include "../../Partition_Interfaces/generic/genericPartitionInterface.h"
#include "../../Partition_Interfaces/generic/genericPartitionInterface.h"


namespace FilesystemInterface
{
    class GenericFilesystemInterface
    {
    private:

    public:
        PartitionInterface::GenericPartitionInterface* partitionInterface = NULL;
        PartitionInterface::PartitionInfo* partitionInfo = NULL;
        FilesystemInterfaceType InterfaceType = FilesystemInterface::Generic;
        GenericFilesystemInterface();
        
        const char* CreateFile(const char* path);
        const char* CreateFolder(const char* path);

        const char* DeleteFile(const char* path);
        const char* DeleteFolder(const char* path);

        const char* CopyFile(const char* oldPath, const char* newPath);
        const char* CopyFolder(const char* oldPath, const char* newPath);

        const char* RenameFile(const char* oldPath, const char* newPath);
        const char* RenameFolder(const char* oldPath, const char* newPath);

        bool FileExists(const char* path);
        bool FolderExists(const char* path);

        FileInfo* GetFileInfo(const char* path);
        FolderInfo* GetFolderInfo(const char* path);

        const char** GetFiles(const char* path);

        const char* ReadFile(const char* path, void** buffer);

        const char* ReadFile(const char* path, uint64_t byteCount, void* buffer);
        const char* WriteFile(const char* path, uint64_t byteCount, void* buffer);

        const char* InitAndSaveFSTable();
        const char* SaveFSTable();
        const char* LoadFSTable();
    };
}