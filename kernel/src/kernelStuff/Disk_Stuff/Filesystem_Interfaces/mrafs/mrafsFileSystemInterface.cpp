#include "mrafsFileSystemInterface.h"
#include "../../../../OSDATA/osdata.h"


namespace FilesystemInterface
{
    MrafsFilesystemInterface::MrafsFilesystemInterface(PartitionInterface::GenericPartitionInterface* partitionInterface, PartitionInterface::PartitionInfo* partitionInfo)
    {
        this->InterfaceType = FilesystemInterfaceType::Mrafs;
        this->partitionInterface = partitionInterface;
        this->partitionInfo = partitionInfo;
        this->partitionInfo->fsInterface = (void*)this;

        fsPartitionList.clear();
        fsFolderList.clear();
        fsFileList.clear();
    }


    const char* MrafsFilesystemInterface::CreateFile(const char* path)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }
    
    const char* MrafsFilesystemInterface::CreateFolder(const char* path)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    const char* MrafsFilesystemInterface::DeleteFile(const char* path)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }

    const char* MrafsFilesystemInterface::DeleteFolder(const char* path)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    const char* MrafsFilesystemInterface::CopyFile(const char* oldPath, const char* newPath)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }

    const char* MrafsFilesystemInterface::CopyFolder(const char* oldPath, const char* newPath)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    const char* MrafsFilesystemInterface::RenameFile(const char* oldPath, const char* newPath)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }

    const char* MrafsFilesystemInterface::RenameFolder(const char* oldPath, const char* newPath)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    bool MrafsFilesystemInterface::FileExists(const char* path)
    {
        if (partitionInterface == NULL)
            return false;
        if (partitionInfo == NULL)
            return false;
        if (partitionInfo->owner != partitionInterface)
            return false;


        return false;
    }

    bool MrafsFilesystemInterface::FolderExists(const char* path)
    {
        if (partitionInterface == NULL)
            return false;
        if (partitionInfo == NULL)
            return false;
        if (partitionInfo->owner != partitionInterface)
            return false;

        return false;
    }


    FileInfo* MrafsFilesystemInterface::GetFileInfo(const char* path)
    {
        if (partitionInterface == NULL)
            return NULL;
        if (partitionInfo == NULL)
            return NULL;
        if (partitionInfo->owner != partitionInterface)
            return NULL;

        return NULL;
    }

    FolderInfo* MrafsFilesystemInterface::GetFolderInfo(const char* path)
    {
        if (partitionInterface == NULL)
            return NULL;
        if (partitionInfo == NULL)
            return NULL;
        if (partitionInfo->owner != partitionInterface)
            return NULL;

        return NULL;
    }


    const char** MrafsFilesystemInterface::GetFiles(const char* path)
    {
        if (partitionInterface == NULL)
            return NULL;
        if (partitionInfo == NULL)
            return NULL;
        if (partitionInfo->owner != partitionInterface)
            return NULL;


        return NULL;
    }


    const char* MrafsFilesystemInterface::ReadFile(const char* path, void** buffer)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }

    const char* MrafsFilesystemInterface::WriteFile(const char* path, void** buffer)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    const char* MrafsFilesystemInterface::ReadFile(const char* path, uint64_t byteCount, void* buffer)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }

    const char* MrafsFilesystemInterface::WriteFile(const char* path, uint64_t byteCount, void* buffer)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }


    const char* MrafsFilesystemInterface::InitAndSaveFSTable()
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;

    
        // Add Code to like free the data from the pointers
        fsPartitionList.clear();
        fsFolderList.clear();
        fsFileList.clear();

        {
            FSPartitionInfo* newInfo = new FSPartitionInfo();
            newInfo->free = true;
            newInfo->locationInBytes = 0;
            newInfo->sizeInBytes = partitionInfo->sizeInBytes;
            fsPartitionList.add(newInfo);
        }

        return SaveFSTable();
    }

    const char* MrafsFilesystemInterface::SaveFSTable()
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        uint64_t totalSize = 0;
        uint32_t fsPartCount = fsPartitionList.getCount();
        uint32_t fsFileCount = fsFileList.getCount();
        uint32_t fsFolderCount = fsFolderList.getCount();
        totalSize = sizeof(totalSize) + sizeof(fsPartCount) + sizeof(fsFileCount) + sizeof(fsFolderCount);

        for (uint32_t i = 0; i < fsPartCount; i++)
        {
            FSPartitionInfo* fsPartInfo = (FSPartitionInfo*)fsPartitionList[i]; 
            totalSize += sizeof(fsPartInfo->sizeInBytes);
            totalSize += sizeof(fsPartInfo->locationInBytes);
            totalSize += sizeof(fsPartInfo->free);
        }
        for (uint32_t i = 0; i < fsFileCount; i++)
        {
            FileInfo* fsFileInfo = fsFileList[i]; 
            totalSize += sizeof(fsFileInfo->baseInfo.hidden);
            totalSize += sizeof(fsFileInfo->baseInfo.systemFile);
            totalSize += sizeof(fsFileInfo->baseInfo.writeProtected);
            totalSize += sizeof(fsFileInfo->baseInfo.pathLen) + fsFileInfo->baseInfo.pathLen;

            totalSize += sizeof(fsFileInfo->locationInBytes);
            totalSize += sizeof(fsFileInfo->sizeInBytes);
        }
        for (uint32_t i = 0; i < fsFolderCount; i++)
        {
            FolderInfo* fsFolderInfo = fsFolderList[i]; 
            totalSize += sizeof(fsFolderInfo->baseInfo.hidden);
            totalSize += sizeof(fsFolderInfo->baseInfo.systemFile);
            totalSize += sizeof(fsFolderInfo->baseInfo.writeProtected);
            totalSize += sizeof(fsFolderInfo->baseInfo.pathLen) + fsFolderInfo->baseInfo.pathLen;
        }
        
        //sizeof(FileInfo);
        //sizeof(FolderInfo);
        //sizeof(FSPartitionInfo);
        osData.mainTerminalWindow->Log("Total Size:         {} Bytes", to_string(totalSize), Colors.yellow);
        osData.mainTerminalWindow->Log("FS Partition Count: {}", to_string((uint64_t)fsPartCount), Colors.yellow);
        osData.mainTerminalWindow->Log("File Count:         {}", to_string((uint64_t)fsFileCount), Colors.yellow);
        osData.mainTerminalWindow->Log("Folder Count:       {}", to_string((uint64_t)fsFolderCount), Colors.yellow);

        if (totalSize > maxFSTableSize)
            return FSCommandResult.ERROR_FS_TABLE_TOO_BIG;

        uint8_t* buffer = (uint8_t*)malloc(totalSize, "Malloc for FS Table Save");
        

        {
            uint8_t* tBuffer = buffer;

            *((uint64_t*)tBuffer) = totalSize;
            tBuffer += 8;
            *((uint32_t*)tBuffer) = fsPartCount;
            tBuffer += 4;
            *((uint32_t*)tBuffer) = fsFileCount;
            tBuffer += 4;
            *((uint32_t*)tBuffer) = fsFolderCount;
            tBuffer += 4;

            for (uint32_t i = 0; i < fsPartCount; i++)
            {
                FSPartitionInfo* fsPartInfo = (FSPartitionInfo*)fsPartitionList[i]; 

                *((uint64_t*)tBuffer) = fsPartInfo->sizeInBytes;
                tBuffer += 8;
                *((uint64_t*)tBuffer) = fsPartInfo->locationInBytes;
                tBuffer += 8;
                *((uint8_t*)tBuffer) = (uint8_t)fsPartInfo->free;
                tBuffer += 1;
            }
            // for (uint32_t i = 0; i < fsFileCount; i++)
            // {
            //     FileInfo* fsFileInfo = fsFileList[i]; 
            //     totalSize += sizeof(fsFileInfo->baseInfo.hidden);
            //     totalSize += sizeof(fsFileInfo->baseInfo.systemFile);
            //     totalSize += sizeof(fsFileInfo->baseInfo.writeProtected);
            //     totalSize += sizeof(fsFileInfo->baseInfo.pathLen) + fsFileInfo->baseInfo.pathLen;

            //     totalSize += sizeof(fsFileInfo->locationInBytes);
            //     totalSize += sizeof(fsFileInfo->sizeInBytes);
            // }
            // for (uint32_t i = 0; i < fsFolderCount; i++)
            // {
            //     FolderInfo* fsFolderInfo = fsFolderList[i]; 
            //     totalSize += sizeof(fsFolderInfo->baseInfo.hidden);
            //     totalSize += sizeof(fsFolderInfo->baseInfo.systemFile);
            //     totalSize += sizeof(fsFolderInfo->baseInfo.writeProtected);
            //     totalSize += sizeof(fsFolderInfo->baseInfo.pathLen) + fsFolderInfo->baseInfo.pathLen;
            // }
        }

        const char* res = partitionInterface->WritePartition(partitionInfo, 0, totalSize, buffer);
        if (res != FSCommandResult.SUCCESS)
            return res;

        free(buffer);
        return FSCommandResult.SUCCESS;
    }

    const char* MrafsFilesystemInterface::LoadFSTable()
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;

        // Add Code to like free the data from the pointers
        fsPartitionList.clear();
        fsFolderList.clear();
        fsFileList.clear();
    



        return FSCommandResult.ERROR_FUNCTION_NOT_IMPLEMENTED;
    }
}