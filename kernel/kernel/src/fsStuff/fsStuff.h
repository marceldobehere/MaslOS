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
    
    FilesystemInterface::GenericFilesystemInterface* GetFsInterfaceFromFullPath(const char* path);



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
