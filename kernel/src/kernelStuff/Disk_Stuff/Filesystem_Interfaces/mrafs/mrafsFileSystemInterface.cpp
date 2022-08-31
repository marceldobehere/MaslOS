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

        if (maxFSTableSize > partitionInfo->sizeInBytes)
            return FSCommandResult.ERROR_PARTITION_TOO_SMALL;

        // {
        //     FSPartitionInfo* newInfo = new FSPartitionInfo();
        //     newInfo->free = true;
        //     newInfo->locationInBytes = 0;
        //     newInfo->sizeInBytes = partitionInfo->sizeInBytes;
        //     fsPartitionList.add(newInfo);
        // }
        {
            FSPartitionInfo* newInfo = new FSPartitionInfo();
            newInfo->free = false;
            newInfo->locationInBytes = 0;
            newInfo->sizeInBytes = maxFSTableSize;
            fsPartitionList.add(newInfo);
        }
        {
            FSPartitionInfo* newInfo = new FSPartitionInfo();
            newInfo->free = false;
            newInfo->locationInBytes = maxFSTableSize;
            newInfo->sizeInBytes = 4;
            fsPartitionList.add(newInfo);
        }
        {
            FSPartitionInfo* newInfo = new FSPartitionInfo();
            newInfo->free = true;
            newInfo->locationInBytes = maxFSTableSize + 4;
            newInfo->sizeInBytes = partitionInfo->sizeInBytes - (maxFSTableSize + 4);
            fsPartitionList.add(newInfo);
        }

        {
            FileInfo* info = new FileInfo(BaseInfo("Test File.txt", false, false, false), 4, maxFSTableSize);
            fsFileList.add(info);
        }

        {
            FolderInfo* info = new FolderInfo(BaseInfo("Test Folder", false, false, false));
            fsFolderList.add(info);
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
            for (uint32_t i = 0; i < fsFileCount; i++)
            {
                FileInfo* fsFileInfo = fsFileList[i]; 

                *((uint8_t*)tBuffer) = (uint8_t)fsFileInfo->baseInfo.hidden;
                tBuffer += 1;
                *((uint8_t*)tBuffer) = (uint8_t)fsFileInfo->baseInfo.systemFile;
                tBuffer += 1;
                *((uint8_t*)tBuffer) = (uint8_t)fsFileInfo->baseInfo.writeProtected;
                tBuffer += 1;

                *((uint16_t*)tBuffer) = (uint16_t)fsFileInfo->baseInfo.pathLen;
                tBuffer += 2;

                //osData.mainTerminalWindow->Log("File Path:          \"{}\"", fsFileInfo->baseInfo.path, Colors.orange);
                //osData.mainTerminalWindow->Log("File Path Len:      \"{}\"", to_string(fsFileInfo->baseInfo.pathLen), Colors.orange);
                for (uint16_t index = 0; index < fsFileInfo->baseInfo.pathLen; index++)
                {
                    *((uint8_t*)tBuffer) = (uint8_t)fsFileInfo->baseInfo.path[index];
                    tBuffer += 1;
                }
                
                *((uint64_t*)tBuffer) = fsFileInfo->locationInBytes;
                tBuffer += 8;

                *((uint64_t*)tBuffer) = fsFileInfo->sizeInBytes;
                tBuffer += 8;
            }
            for (uint32_t i = 0; i < fsFolderCount; i++)
            {
                FolderInfo* fsFolderInfo = fsFolderList[i]; 

                *((uint8_t*)tBuffer) = (uint8_t)fsFolderInfo->baseInfo.hidden;
                tBuffer += 1;
                *((uint8_t*)tBuffer) = (uint8_t)fsFolderInfo->baseInfo.systemFile;
                tBuffer += 1;
                *((uint8_t*)tBuffer) = (uint8_t)fsFolderInfo->baseInfo.writeProtected;
                tBuffer += 1;
                *((uint16_t*)tBuffer) = (uint16_t)fsFolderInfo->baseInfo.pathLen;
                tBuffer += 2;
                
                //osData.mainTerminalWindow->Log("Folder Path:        \"{}\"", fsFolderInfo->baseInfo.path, Colors.orange);
                //osData.mainTerminalWindow->Log("Folder Path Len:    \"{}\"", to_string(fsFolderInfo->baseInfo.pathLen), Colors.orange);
                for (uint16_t index = 0; index < fsFolderInfo->baseInfo.pathLen; index++)
                {
                    *((uint8_t*)tBuffer) = (uint8_t)fsFolderInfo->baseInfo.path[index];
                    tBuffer += 1;
                }
            }

            uint64_t diff = ((((uint64_t)buffer ) + totalSize) - ((uint64_t)tBuffer));
            if (diff != 0)
                osData.mainTerminalWindow->Log("Diff:               {}", to_string(diff), Colors.red);
            
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
    
        uint64_t totalSize = 0;
        uint32_t fsPartCount = 0;
        uint32_t fsFileCount = 0;
        uint32_t fsFolderCount = 0;
        uint64_t addrOffset = 0;

        {
            uint8_t* buffer = (uint8_t*)malloc(20, "mini alloc for reading fs data");
            const char* res = partitionInterface->ReadPartition(partitionInfo, 0, 20, buffer);
            if (res != FSCommandResult.SUCCESS)
            {
                free(buffer);
                return res;
            }
            uint8_t* tBuffer = buffer;

            totalSize = *((uint64_t*)tBuffer);
            tBuffer += 8;
            fsPartCount = *((uint32_t*)tBuffer);
            tBuffer += 4;
            fsFileCount = *((uint32_t*)tBuffer);
            tBuffer += 4;
            fsFolderCount = *((uint32_t*)tBuffer);
            tBuffer += 4;


            addrOffset = ((uint64_t)tBuffer - (uint64_t)buffer);
            free(buffer);
        }

        osData.mainTerminalWindow->Log("Total Size:         {} Bytes", to_string(totalSize), Colors.yellow);
        osData.mainTerminalWindow->Log("FS Partition Count: {}", to_string((uint64_t)fsPartCount), Colors.yellow);
        osData.mainTerminalWindow->Log("File Count:         {}", to_string((uint64_t)fsFileCount), Colors.yellow);
        osData.mainTerminalWindow->Log("Folder Count:       {}", to_string((uint64_t)fsFolderCount), Colors.yellow);

        if (totalSize > maxFSTableSize)
            return FSCommandResult.ERROR_FS_TABLE_TOO_BIG;


    {
        uint8_t* buffer = (uint8_t*)malloc((totalSize - addrOffset), "Malloc for FS Table Save");
        const char* res = partitionInterface->ReadPartition(partitionInfo, addrOffset, (totalSize - addrOffset), buffer);
        if (res != FSCommandResult.SUCCESS)
        {
            free(buffer);
            return res;
        }
        uint8_t* tBuffer = buffer;

        for (uint32_t i = 0; i < fsPartCount; i++)
        {
            FSPartitionInfo* fsPartInfo = (FSPartitionInfo*)malloc(sizeof(FSPartitionInfo), "Filesystem Partition Info malloc"); 
            fsPartitionList.add(fsPartInfo);

            fsPartInfo->sizeInBytes = *((uint64_t*)tBuffer);
            tBuffer += 8;
            fsPartInfo->locationInBytes = *((uint64_t*)tBuffer);
            tBuffer += 8;
            fsPartInfo->free = *((uint8_t*)tBuffer);
            tBuffer += 1;
        }

        for (uint32_t i = 0; i < fsFileCount; i++)
        {
            FileInfo* fsFileInfo = (FileInfo*)malloc(sizeof(FileInfo), "Filesystem File Info malloc");
            fsFileList.add(fsFileInfo);

            fsFileInfo->baseInfo.hidden = *((uint8_t*)tBuffer);
            tBuffer += 1;
            fsFileInfo->baseInfo.systemFile = *((uint8_t*)tBuffer);
            tBuffer += 1;
            fsFileInfo->baseInfo.writeProtected = *((uint8_t*)tBuffer);
            tBuffer += 1;

            fsFileInfo->baseInfo.pathLen = *((uint16_t*)tBuffer);
            tBuffer += 2;

            //osData.mainTerminalWindow->Log("File Path:          \"{}\"", fsFileInfo->baseInfo.path, Colors.orange);
            //osData.mainTerminalWindow->Log("File Path Len:      \"{}\"", to_string(fsFileInfo->baseInfo.pathLen), Colors.orange);
            fsFileInfo->baseInfo.path = (char*)malloc(fsFileInfo->baseInfo.pathLen + 1, "Malloc for fs path string");
            ((char*)fsFileInfo->baseInfo.path)[fsFileInfo->baseInfo.pathLen] = (uint8_t)0;
            for (uint16_t index = 0; index < fsFileInfo->baseInfo.pathLen; index++)
            {
                ((char*)fsFileInfo->baseInfo.path)[index] = *((uint8_t*)tBuffer);
                tBuffer += 1;
            }
            
            fsFileInfo->locationInBytes = *((uint64_t*)tBuffer);
            tBuffer += 8;

            fsFileInfo->sizeInBytes = *((uint64_t*)tBuffer);
            tBuffer += 8;
        }

        for (uint32_t i = 0; i < fsFolderCount; i++)
        {
            FolderInfo* fsFolderInfo = (FolderInfo*)malloc(sizeof(FolderInfo), "Filesystem Folder Info malloc");
            fsFolderList.add(fsFolderInfo); 

            fsFolderInfo->baseInfo.hidden = *((uint8_t*)tBuffer);
            tBuffer += 1;
            fsFolderInfo->baseInfo.systemFile = *((uint8_t*)tBuffer);
            tBuffer += 1;
            fsFolderInfo->baseInfo.writeProtected = *((uint8_t*)tBuffer);
            tBuffer += 1;
            fsFolderInfo->baseInfo.pathLen = *((uint16_t*)tBuffer);
            tBuffer += 2;
            
            //osData.mainTerminalWindow->Log("Folder Path:        \"{}\"", fsFolderInfo->baseInfo.path, Colors.orange);
            //osData.mainTerminalWindow->Log("Folder Path Len:    \"{}\"", to_string(fsFolderInfo->baseInfo.pathLen), Colors.orange);
            fsFolderInfo->baseInfo.path = (char*)malloc(fsFolderInfo->baseInfo.pathLen + 1, "Malloc for fs path string");
            ((char*)fsFolderInfo->baseInfo.path)[fsFolderInfo->baseInfo.pathLen] = (uint8_t)0;
            for (uint16_t index = 0; index < fsFolderInfo->baseInfo.pathLen; index++)
            {
                ((char*)fsFolderInfo->baseInfo.path)[index] = *((uint8_t*)tBuffer);
                tBuffer += 1;
            }
        }


        uint64_t diff = ((((uint64_t)buffer) + (totalSize - addrOffset)) - ((uint64_t)tBuffer));
        if (diff != 0)
            osData.mainTerminalWindow->Log("Diff:               {}", to_string(diff), Colors.red);


        free(buffer);
    }

        return FSCommandResult.SUCCESS;
    }
}