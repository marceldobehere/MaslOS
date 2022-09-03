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
    void MrafsFilesystemInterface::ClearLists()
    {
        {
            uint64_t count = fsFileList.getCount();
            while (count--)
            {
                fsFileList[0]->Destroy();
                fsFileList.removeAt(0);
            }
        }

        {
            uint64_t count = fsFolderList.getCount();
            while (count--)
            {
                fsFolderList[0]->Destroy();
                fsFolderList.removeAt(0);
            }
        }

        fsPartitionList.clear();
    }

    int64_t MrafsFilesystemInterface::GetIndexOfPartitionFromLocation(uint64_t location)
    {
        if (partitionInterface == NULL)
            return -1;
        if (partitionInfo == NULL)
            return -1;
        if (partitionInfo->owner != partitionInterface)
            return -1;

        uint64_t count = fsPartitionList.getCount();
        for (uint64_t index = 0; index < count; index++)
            if (((FSPartitionInfo*)fsPartitionList[index])->locationInBytes == location)
                return index;

        return -1;
    }

    int64_t MrafsFilesystemInterface::GetIndexOfFreeFilePartition(uint64_t sizeInBytes)
    {
        if (partitionInterface == NULL)
            return -1;
        if (partitionInfo == NULL)
            return -1;
        if (partitionInfo->owner != partitionInterface)
            return -1;
        //osData.mainTerminalWindow->Log("BRUH 0");
        uint64_t count = fsPartitionList.getCount();
        //osData.mainTerminalWindow->Log("BRUH 0, Count: {}", to_string(count), Colors.yellow);
        for (uint64_t index = 0; index < count; index++)
        {
            //osData.mainTerminalWindow->Log("BRUH 1, Index: {}", to_string(index), Colors.yellow);
            FSPartitionInfo* part = (FSPartitionInfo*)fsPartitionList[index];
            //osData.mainTerminalWindow->Log("BRUH 1, Free: {}", part->free ? "true" : "false", Colors.yellow);
            if (!part->free)
                continue;
            //osData.mainTerminalWindow->Log("BRUH 1, Size: {}", to_string(part->sizeInBytes), Colors.yellow);
            if (part->sizeInBytes < sizeInBytes)
                continue;
            //osData.mainTerminalWindow->Log("BRUH 2");
                
            return index;
        }

        return -1;
    }

    MrafsFilesystemInterface::FSPartitionInfo* MrafsFilesystemInterface::CreateFilePartition(uint64_t size)
    {
        //osData.mainTerminalWindow->Log("AAA 0");

        if (partitionInterface == NULL)
            return NULL;
        if (partitionInfo == NULL)
            return NULL;
        if (partitionInfo->owner != partitionInterface)
            return NULL;

        //osData.mainTerminalWindow->Log("AAA 1");
        //osData.mainTerminalWindow->Log("BRUH Y, Count: {}", to_string(fsPartitionList.getCount()), Colors.yellow);
        int64_t fIndex = GetIndexOfFreeFilePartition(size);
        if (fIndex == -1)
            return NULL;
        
        //osData.mainTerminalWindow->Log("AAA 2");

        FSPartitionInfo* info = (FSPartitionInfo*)fsPartitionList[fIndex];
        info->free = false;
        if (info->sizeInBytes > size)
        {
            if (ResizeFilePartition(info, size) != FSCommandResult.SUCCESS)
                return NULL;
        }

        return info;
    }

    const char* MrafsFilesystemInterface::ResizeFilePartition(FSPartitionInfo* partition, uint64_t newSize)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;
        if (partition == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        int64_t pIndex = fsPartitionList.getIndexOf(partition);
        if (pIndex == -1)
            return FSCommandResult.ERROR_PARTITION_NOT_FOUND;
        
        if (newSize == partition->sizeInBytes)
            return FSCommandResult.SUCCESS;
        
        if (newSize > partition->sizeInBytes)
        {
            if (pIndex >= fsPartitionList.getCount() - 1)
                return FSCommandResult.ERROR_PARTITION_TOO_SMALL;
            FSPartitionInfo* info = ( FSPartitionInfo*)fsPartitionList[pIndex + 1];
            if (!info->free)
                return FSCommandResult.ERROR_PARTITION_TOO_SMALL;

            if (newSize == (partition->sizeInBytes + info->sizeInBytes))
            {
                partition->sizeInBytes = newSize;
                free(fsPartitionList[pIndex + 1]);
                fsPartitionList.removeAt(pIndex + 1);
                return FSCommandResult.SUCCESS;
            }
            else
            {
                uint64_t sizeDiff = newSize - partition->sizeInBytes;
                info->sizeInBytes -= sizeDiff;
                info->locationInBytes += sizeDiff;
                partition->sizeInBytes += sizeDiff;
            }
            
            return FSCommandResult.SUCCESS;
        }

        if (newSize < partition->sizeInBytes)
        {
            uint64_t sizeDiff = partition->sizeInBytes - newSize;

            partition->sizeInBytes -= sizeDiff;

            FSPartitionInfo* info = (FSPartitionInfo*)malloc(sizeof(FSPartitionInfo), "malloc for FSPartitionInfo");
            info->free = true;
            info->sizeInBytes = sizeDiff;
            info->locationInBytes = partition->locationInBytes + partition->sizeInBytes;
            fsPartitionList.insertAt(info, pIndex + 1);
            return FSCommandResult.SUCCESS;
        }


        return FSCommandResult.SUCCESS;
    }

    const char* MrafsFilesystemInterface::DeleteFilePartition(FSPartitionInfo* partition)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;
        if (partition == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        int64_t pIndex = fsPartitionList.getIndexOf(partition);
        if (pIndex == -1)
            return FSCommandResult.ERROR_PARTITION_NOT_FOUND;

        partition->free = true;

        if (pIndex < fsPartitionList.getCount() - 1)
        {
            FSPartitionInfo* pTemp = (FSPartitionInfo*)fsPartitionList[pIndex + 1];
            if (pTemp->free)
            {
                const char* res = ResizeFilePartition(partition, partition->sizeInBytes + pTemp->sizeInBytes);
                if (res != FSCommandResult.SUCCESS)
                    return res;
            }
        }

        if (pIndex > 0)
        {
            FSPartitionInfo* pTemp = (FSPartitionInfo*)fsPartitionList[pIndex - 1];
            if (pTemp->free)
            {
                const char* res = ResizeFilePartition(pTemp, partition->sizeInBytes + pTemp->sizeInBytes);
                if (res != FSCommandResult.SUCCESS)
                    return res;
            }
        }
        
        return FSCommandResult.SUCCESS;
    }

    FileInfo* MrafsFilesystemInterface::GetFile(const char* path)
    {
        uint64_t count = fsFileList.getCount();
        for (uint64_t index = 0; index < count; index++)
        {
            FileInfo* info = fsFileList[index];
            if (StrEquals(info->baseInfo.path, path))
                return info;
        }

        return NULL;
    }

    FolderInfo* MrafsFilesystemInterface::GetFolder(const char* path)
    {
        uint64_t count = fsFolderList.getCount();
        for (uint64_t index = 0; index < count; index++)
        {
            FolderInfo* info = fsFolderList[index];
            if (StrEquals(info->baseInfo.path, path))
                return info;
        }

        return NULL;
    }


    const char* MrafsFilesystemInterface::CreateFile(const char* path)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;
        
        FileInfo* file = GetFile(path);
        if (file != NULL)
            return FSCommandResult.ERROR_FILE_ALREADY_EXISTS;
        
        file = new FileInfo(BaseInfo(path, false, false, false), 0, 0);
        fsFileList.add(file);

        return SaveFSTable();//return FSCommandResult.SUCCESS;
    }
    
    const char* MrafsFilesystemInterface::CreateFolder(const char* path)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;

        FolderInfo* folder = GetFolder(path);
        if (folder != NULL)
            return FSCommandResult.ERROR_FILE_ALREADY_EXISTS;
        
        folder = new FolderInfo(BaseInfo(path, false, false, false));
        fsFolderList.add(folder);

        return SaveFSTable();//return FSCommandResult.SUCCESS;
    }


    const char* MrafsFilesystemInterface::DeleteFile(const char* path)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;

        FileInfo* file = GetFile(path);
        if (file == NULL)
            return FSCommandResult.ERROR_FILE_NOT_FOUND;

        int64_t fIndex = fsFileList.getIndexOf(file);
        if (fIndex == -1)
            return FSCommandResult.ERROR_FILE_NOT_FOUND;
        
        if (file->sizeInBytes != 0)
        {
            int64_t pIndex = GetIndexOfPartitionFromLocation(file->locationInBytes);
            if (pIndex == -1)
                return FSCommandResult.ERROR_FILE_NOT_FOUND;
            
            FSPartitionInfo* info = (FSPartitionInfo*) fsPartitionList[pIndex];
            
            if (info->sizeInBytes != 0)
                DeleteFilePartition(info);
        }
        
        file->sizeInBytes = 0;
        file->locationInBytes = 0;

        file->Destroy();
        free(file);
        fsFileList.removeAt(fIndex);

        
        return SaveFSTable();//return FSCommandResult.SUCCESS;
    }

    const char* MrafsFilesystemInterface::DeleteFolder(const char* path)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;

        FolderInfo* folder = GetFolder(path);
        if (folder == NULL)
            return FSCommandResult.ERROR_FOLDER_NOT_FOUND;

        int64_t fIndex = fsFolderList.getIndexOf(folder);
        if (fIndex == -1)
            return FSCommandResult.ERROR_FOLDER_NOT_FOUND;


        {
            uint64_t fileCount = 0;
            const char** files = GetFiles(folder->baseInfo.path, &fileCount);
            if (fileCount != 0)
            {
                for (int i = 0; i < fileCount; i++)
                {
                    DeleteFile(files[i]);
                    free((void*)files[i]);
                }
                free(files);
            }
        }

        {
            uint64_t folderCount = 0;
            const char** folders = GetFolders(folder->baseInfo.path, &folderCount);
            if (folderCount != 0)
            {
                for (int i = 0; i < folderCount; i++)
                {
                    DeleteFolder(folders[i]);
                    free((void*)folders[i]);
                }
                free(folders);
            }
        }


        // const char* basePath = StrCombine(folder->baseInfo.path, "/");

        // {
        //     uint64_t count = fsFileList.getCount();
        //     for (int index = 0; index < count; index++)
        //     {
        //         FileInfo* info = fsFileList[index];
        //         if (StrStartsWith(info->baseInfo.path, basePath))
        //         {
        //             DeleteFile(info->baseInfo.path);
        //             index = -1;
        //         }
        //     }
        // }

        // {
        //     uint64_t count = fsFolderList.getCount();
        //     for (int index = 0; index < count; index++)
        //     {
        //         FolderInfo* info = fsFolderList[index];
        //         if (StrStartsWith(info->baseInfo.path, basePath))
        //         {
        //             DeleteFolder(info->baseInfo.path);
        //             index = -1;
        //         }
        //     }
        // }

        // free((void*)basePath);

        folder->Destroy();
        free(folder);
        fsFolderList.removeAt(fIndex);
        
        return SaveFSTable();//return FSCommandResult.SUCCESS;
    }


    const char* MrafsFilesystemInterface::CopyFile(const char* oldPath, const char* newPath)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;

        if (StrEquals(oldPath, newPath))
            return FSCommandResult.ERROR_PATH_IS_THE_SAME;
        
        FileInfo* file = GetFile(oldPath);
        if (file == NULL)
            return FSCommandResult.ERROR_FILE_NOT_FOUND;
        if (FileExists(newPath))
            return FSCommandResult.ERROR_FILE_ALREADY_EXISTS;
        if (FolderExists(newPath))
            return FSCommandResult.ERROR_FOLDER_ALREADY_EXISTS;
        
        {
            const char* res = CreateFile(newPath);
            if (res != FSCommandResult.SUCCESS)
                return res;
        }

        FileInfo* newFile = GetFile(newPath);
        if (newFile == NULL)
            return FSCommandResult.ERROR_FILE_NOT_FOUND;

        {
            const char* tempPath = newFile->baseInfo.path;
            uint16_t tempLen = newFile->baseInfo.pathLen;
            newFile->baseInfo = file->baseInfo;
            newFile->baseInfo.path = tempPath;
            newFile->baseInfo.pathLen = tempLen;
        }

        {
            uint8_t* buffer = NULL;
            uint64_t byteCount = ReadFile(oldPath, (void**)&buffer);
            if (byteCount != 0)
            {
                WriteFile(newPath, byteCount, buffer);
                free(buffer);
            }
        }

        return FSCommandResult.SUCCESS;
    }

    const char* MrafsFilesystemInterface::CopyFolder(const char* oldPath, const char* newPath)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;

        if (StrEquals(oldPath, newPath))
            return FSCommandResult.ERROR_PATH_IS_THE_SAME;


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

        if (StrEquals(oldPath, newPath))
            return FSCommandResult.ERROR_PATH_IS_THE_SAME;

        FileInfo* file = GetFile(oldPath);
        if (file == NULL)
            return FSCommandResult.ERROR_FILE_NOT_FOUND;
        if (FileExists(newPath))
            return FSCommandResult.ERROR_FILE_ALREADY_EXISTS;
        if (FolderExists(newPath))
            return FSCommandResult.ERROR_FOLDER_ALREADY_EXISTS;
        
        free((void*)file->baseInfo.path);
        file->baseInfo.path = StrCopy(newPath);
        file->baseInfo.pathLen = StrLen(file->baseInfo.path);

        return SaveFSTable();
    }

    const char* MrafsFilesystemInterface::RenameFolder(const char* oldPath, const char* newPath)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;

        if (StrEquals(oldPath, newPath))
            return FSCommandResult.ERROR_PATH_IS_THE_SAME;


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

        return GetFile(path) != NULL;
    }

    bool MrafsFilesystemInterface::FolderExists(const char* path)
    {
        if (partitionInterface == NULL)
            return false;
        if (partitionInfo == NULL)
            return false;
        if (partitionInfo->owner != partitionInterface)
            return false;

        return GetFolder(path) != NULL;
    }


    FileInfo* MrafsFilesystemInterface::GetFileInfo(const char* path)
    {
        if (partitionInterface == NULL)
            return NULL;
        if (partitionInfo == NULL)
            return NULL;
        if (partitionInfo->owner != partitionInterface)
            return NULL;

        return GetFile(path);
    }

    FolderInfo* MrafsFilesystemInterface::GetFolderInfo(const char* path)
    {
        if (partitionInterface == NULL)
            return NULL;
        if (partitionInfo == NULL)
            return NULL;
        if (partitionInfo->owner != partitionInterface)
            return NULL;

        return GetFolder(path);
    }


    const char** MrafsFilesystemInterface::GetFiles(const char* path, uint64_t* outCount)
    {
        *outCount = 0;
        if (partitionInterface == NULL)
            return NULL;
        if (partitionInfo == NULL)
            return NULL;
        if (partitionInfo->owner != partitionInterface)
            return NULL;


        if (StrEquals(path, "/"))
        {
            //osData.mainTerminalWindow->Log("Slash: {}", to_string(baseSlashCount), Colors.yellow);
            uint64_t fCount = 0;
            uint64_t fSize = 0;
            {
                uint64_t count = fsFileList.getCount();
                for (int index = 0; index < count; index++)
                {
                    FileInfo* info = fsFileList[index];
                    int diff = StrCountChr(info->baseInfo.path, '/'); 
                    //osData.mainTerminalWindow->Log("Slash Diff: {}", to_string(diff), Colors.yellow);
                    if (diff != 0)
                        continue;
                    
                    fCount++;
                    fSize += info->baseInfo.pathLen + 1;
                }
            }
            const char** strList = NULL;
            if (fCount != 0)
            {
                strList = (const char**)malloc(sizeof(const char*) * fCount, "Malloc for File list");
                fCount = 0;
                {
                    uint64_t count = fsFileList.getCount();
                    for (int index = 0; index < count; index++)
                    {
                        FileInfo* info = fsFileList[index];
                        int diff = StrCountChr(info->baseInfo.path, '/'); 
                        if (diff != 0)
                            continue;
                        
                        strList[fCount] = StrCopy(info->baseInfo.path);
                        fCount++;
                    }
                }
            }
            
            *outCount = fCount;
            return strList;
        }
        else
        {
            FolderInfo* folder = GetFolder(path);
            if (folder == NULL)
                return NULL;
            
            const char* basePath = StrCombine(folder->baseInfo.path, "/");
            int baseSlashCount = StrCountChr(basePath, '/');
            //osData.mainTerminalWindow->Log(basePath);
            //osData.mainTerminalWindow->Log("Slash: {}", to_string(baseSlashCount), Colors.yellow);
            uint64_t fCount = 0;
            uint64_t fSize = 0;
            {
                uint64_t count = fsFileList.getCount();
                for (int index = 0; index < count; index++)
                {
                    FileInfo* info = fsFileList[index];
                    int diff = StrCountChr(info->baseInfo.path, '/') - baseSlashCount; 
                    //osData.mainTerminalWindow->Log("Slash Diff: {}", to_string(diff), Colors.yellow);
                    if (diff != 0)
                        continue;
                    if (StrStartsWith(info->baseInfo.path, basePath))
                    {
                        fCount++;
                        fSize += info->baseInfo.pathLen + 1;
                    }
                }
            }
            const char** strList = NULL;
            if (fCount != 0)
            {
                strList = (const char**)malloc(sizeof(const char*) * fCount, "Malloc for File list");
                fCount = 0;
                {
                    uint64_t count = fsFileList.getCount();
                    for (int index = 0; index < count; index++)
                    {
                        FileInfo* info = fsFileList[index];
                        int diff = StrCountChr(info->baseInfo.path, '/') - baseSlashCount; 
                        if (diff != 0)
                            continue;
                        if (StrStartsWith(info->baseInfo.path, basePath))
                        {
                            strList[fCount] = StrCopy(info->baseInfo.path);
                            fCount++;
                        }
                    }
                }
            }
            
            
            *outCount = fCount;
            free((void*)basePath);
            return strList;
        }

        return NULL;
    }

    const char** MrafsFilesystemInterface::GetFolders(const char* path, uint64_t* outCount)
    {
        *outCount = 0;
        if (partitionInterface == NULL)
            return NULL;
        if (partitionInfo == NULL)
            return NULL;
        if (partitionInfo->owner != partitionInterface)
            return NULL;

        if (StrEquals(path, "/"))
        {
            uint64_t fCount = 0;
            uint64_t fSize = 0;
            {
                uint64_t count = fsFolderList.getCount();
                for (int index = 0; index < count; index++)
                {
                    FolderInfo* info = fsFolderList[index];
                    int diff = StrCountChr(info->baseInfo.path, '/'); 
                    if (diff != 0)
                        continue;
                    
                    fCount++;
                    fSize += info->baseInfo.pathLen + 1;
                }
            }
            const char** strList = NULL;
            if (fCount != 0)
            {
                strList = (const char**)malloc(sizeof(const char*) * fCount, "Malloc for File list");
                fCount = 0;
                {
                    uint64_t count = fsFolderList.getCount();
                    for (int index = 0; index < count; index++)
                    {
                        FolderInfo* info = fsFolderList[index];
                        int diff = StrCountChr(info->baseInfo.path, '/'); 
                        if (diff != 0)
                            continue;

                        strList[fCount] = StrCopy(info->baseInfo.path);
                        fCount++;
                    }
                }
            }

            *outCount = fCount;

            return strList;
        }
        else
        {
            FolderInfo* folder = GetFolder(path);
            if (folder == NULL)
                return NULL;
            
            const char* basePath = StrCombine(folder->baseInfo.path, "/");
            int baseSlashCount = StrCountChr(basePath, '/');

            uint64_t fCount = 0;
            uint64_t fSize = 0;
            {
                uint64_t count = fsFolderList.getCount();
                for (int index = 0; index < count; index++)
                {
                    FolderInfo* info = fsFolderList[index];
                    int diff = StrCountChr(info->baseInfo.path, '/') - baseSlashCount; 
                    if (diff != 0)
                        continue;
                    if (StrStartsWith(info->baseInfo.path, basePath))
                    {
                        fCount++;
                        fSize += info->baseInfo.pathLen + 1;
                    }
                }
            }
            const char** strList = NULL;
            if (fCount != 0)
            {
                strList = (const char**)malloc(sizeof(const char*) * fCount, "Malloc for File list");
                fCount = 0;
                {
                    uint64_t count = fsFolderList.getCount();
                    for (int index = 0; index < count; index++)
                    {
                        FolderInfo* info = fsFolderList[index];
                        int diff = StrCountChr(info->baseInfo.path, '/') - baseSlashCount; 
                        if (diff != 0)
                            continue;
                        if (StrStartsWith(info->baseInfo.path, basePath))
                        {
                            strList[fCount] = StrCopy(info->baseInfo.path);
                            fCount++;
                        }
                    }
                }
            }

            *outCount = fCount;
            free((void*)basePath);

            return strList;
        }

        return NULL;
    }


    uint64_t MrafsFilesystemInterface::ReadFile(const char* path, void** buffer)
    {
        if (partitionInterface == NULL)
            return 0;
        if (partitionInfo == NULL)
            return 0;
        if (partitionInfo->owner != partitionInterface)
            return 0;
        
        FileInfo* info = GetFile(path);
        if (info == NULL)
            return 0;

        uint8_t* data = (uint8_t*)malloc(info->sizeInBytes, "Malloc for File read");

        *((uint8_t**)buffer) = data;

        const char* res = partitionInterface->ReadPartition(partitionInfo, info->locationInBytes, info->sizeInBytes, data);
        if (res != PartitionInterface::CommandResult.SUCCESS)
            return 0;

        return info->sizeInBytes;
    }



    const char* MrafsFilesystemInterface::ReadFile(const char* path, uint64_t byteCount, void* buffer)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;

        FileInfo* info = GetFile(path);
        if (info == NULL)
            return FSCommandResult.ERROR_FILE_NOT_FOUND;

        if (byteCount > info->sizeInBytes)
            return FSCommandResult.ERROR_FILE_TOO_SMALL;

        const char* res = partitionInterface->ReadPartition(partitionInfo, info->locationInBytes, byteCount, buffer);
        if (res != PartitionInterface::CommandResult.SUCCESS)
            return res;
        
        return FSCommandResult.SUCCESS;
    }

    const char* MrafsFilesystemInterface::WriteFile(const char* path, uint64_t byteCount, void* buffer)
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;

        FileInfo* info = GetFile(path);
        if (info == NULL)
            return FSCommandResult.ERROR_FILE_NOT_FOUND;

        //osData.mainTerminalWindow->Log("BRUH X, Count: {}", to_string(fsPartitionList.getCount()), Colors.yellow);
        //osData.mainTerminalWindow->Log("BRUH X2, Count: {}", to_string(fsPartitionList.getCount()), Colors.yellow);

        if (byteCount == info->sizeInBytes)
        {
            const char* res = partitionInterface->WritePartition(partitionInfo, info->locationInBytes, byteCount, buffer);
            if (res != PartitionInterface::CommandResult.SUCCESS)
                return res;
            return SaveFSTable();//return FSCommandResult.SUCCESS;       
        }
        else
        {
            if(info->sizeInBytes != 0)
            {
                //osData.mainTerminalWindow->Log("BRUH X3, Count: {}", to_string(fsPartitionList.getCount()), Colors.yellow);
                int64_t pIndex = GetIndexOfPartitionFromLocation(info->locationInBytes);
                if (pIndex == -1)
                    return FSCommandResult.ERROR_PARTITION_NOT_FOUND;
                FSPartitionInfo* partInfo = (FSPartitionInfo*)fsPartitionList[pIndex];
                //osData.mainTerminalWindow->Log("BRUH X3, Size: {}", to_string(partInfo->sizeInBytes), Colors.yellow);
                if (partInfo->sizeInBytes != 0)
                    DeleteFilePartition(partInfo);
            }
            //osData.mainTerminalWindow->Log("BRUH X4, Count: {}", to_string(fsPartitionList.getCount()), Colors.yellow);
            FSPartitionInfo* nPartInfo = CreateFilePartition(byteCount);
            //osData.mainTerminalWindow->Log("BRUH X5, Count: {}", to_string(fsPartitionList.getCount()), Colors.yellow);
            if (nPartInfo == NULL)
                return FSCommandResult.ERROR_PARTITION_TOO_SMALL;
            info->locationInBytes = nPartInfo->locationInBytes;
            info->sizeInBytes = byteCount;

            const char* res = partitionInterface->WritePartition(partitionInfo, info->locationInBytes, byteCount, buffer);
            if (res != PartitionInterface::CommandResult.SUCCESS)
                return res;

            return SaveFSTable();//return FSCommandResult.SUCCESS;    
        }
        

        return SaveFSTable();//return FSCommandResult.SUCCESS;
    }


    const char* MrafsFilesystemInterface::InitAndSaveFSTable()
    {
        if (partitionInterface == NULL)
            return FSCommandResult.ERROR_NO_PARTITION_INTERFACE;
        if (partitionInfo == NULL)
            return FSCommandResult.ERROR_NO_PARTITION;
        if (partitionInfo->owner != partitionInterface)
            return FSCommandResult.ERROR_INVALID_PARTITION_OWNER;


        ClearLists();

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
        // {
        //     FSPartitionInfo* newInfo = new FSPartitionInfo();
        //     newInfo->free = false;
        //     newInfo->locationInBytes = maxFSTableSize;
        //     newInfo->sizeInBytes = 4;
        //     fsPartitionList.add(newInfo);
        // }
        // {
        //     FSPartitionInfo* newInfo = new FSPartitionInfo();
        //     newInfo->free = true;
        //     newInfo->locationInBytes = maxFSTableSize + 4;
        //     newInfo->sizeInBytes = partitionInfo->sizeInBytes - (maxFSTableSize + 4);
        //     fsPartitionList.add(newInfo);
        // }
        {
            FSPartitionInfo* newInfo = new FSPartitionInfo();
            newInfo->free = true;
            newInfo->locationInBytes = maxFSTableSize;
            newInfo->sizeInBytes = partitionInfo->sizeInBytes - maxFSTableSize;
            fsPartitionList.add(newInfo);
        }

        // {
        //     FileInfo* info = new FileInfo(BaseInfo("Test File.txt", false, false, false), 4, maxFSTableSize);
        //     fsFileList.add(info);
        // }

        // {
        //     FolderInfo* info = new FolderInfo(BaseInfo("Test Folder", false, false, false));
        //     fsFolderList.add(info);
        // }

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
        // osData.mainTerminalWindow->Log("Total Size:         {} Bytes", to_string(totalSize), Colors.yellow);
        // osData.mainTerminalWindow->Log("FS Partition Count: {}", to_string((uint64_t)fsPartCount), Colors.yellow);
        // osData.mainTerminalWindow->Log("File Count:         {}", to_string((uint64_t)fsFileCount), Colors.yellow);
        // osData.mainTerminalWindow->Log("Folder Count:       {}", to_string((uint64_t)fsFolderCount), Colors.yellow);

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

        ClearLists();
    
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

        // osData.mainTerminalWindow->Log("Total Size:         {} Bytes", to_string(totalSize), Colors.yellow);
        // osData.mainTerminalWindow->Log("FS Partition Count: {}", to_string((uint64_t)fsPartCount), Colors.yellow);
        // osData.mainTerminalWindow->Log("File Count:         {}", to_string((uint64_t)fsFileCount), Colors.yellow);
        // osData.mainTerminalWindow->Log("Folder Count:       {}", to_string((uint64_t)fsFolderCount), Colors.yellow);

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