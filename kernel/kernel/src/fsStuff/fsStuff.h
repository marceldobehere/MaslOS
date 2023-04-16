#pragma once

#include "../kernelStuff/Disk_Stuff/Disk_Interfaces/ram/ramDiskInterface.h"
#include "../kernelStuff/Disk_Stuff/Disk_Interfaces/file/fileDiskInterface.h"
#include "../kernelStuff/Disk_Stuff/Partition_Interfaces/mraps/mrapsPartitionInterface.h"
#include "../kernelStuff/Disk_Stuff/Filesystem_Interfaces/mrafs/mrafsFileSystemInterface.h"


namespace FS_STUFF
{
    char* GetDriveNameFromFullPath(const char* path);
    char* GetFilePathFromFullPath(const char* path);
    char* GetFolderPathFromFullPath(const char* path);

    bool ReadFileFromFullPath(const char* path, char** resBuffer, int* resBufferLen);
    bool WriteFileToFullPath(const char* path, char* buffer, int bufferLen, bool createIfNotExists);

    FilesystemInterface::GenericFilesystemInterface* GetFsInterfaceFromFullPath(const char* path);
    FilesystemInterface::FileInfo* GetFileInfoFromFullPath(const char* path);

    bool OpenFile(const char* path);
    void RunTerminalCommand(const char* terminalCommand, const char* terminalWindowTitle, bool hideTerminalWindow, bool autoCloseTerminalWindow);


/*
Println(window, "Avaiable Drives:", Colors.bgreen);
        for (int i = 0; i < osData.diskInterfaces.getCount(); i++)
        {
            DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[i];
            if (diskInterface->partitionInterface == NULL)
                continue;
            PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
            for (int i2 = 0; i2 < partInterface->partitionList.getCount(); i2++)
            {
                PartitionInterface::PartitionInfo* partInfo = partInterface->partitionList[i2];
                if (!partInfo->hidden && partInfo->type == PartitionInterface::PartitionType::Normal)
                {
                    Println(window, " - Drive: \"{}\"", partInfo->driveName, Colors.bgreen);
                }
            }
        }
*/


}
