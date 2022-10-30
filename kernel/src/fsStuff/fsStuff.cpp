#include "fsStuff.h"

#include <stdint.h>
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"
#include "../kernelStuff/stuff/cstr.h"
#include "../cmdParsing/cstrTools.h"
#include "../OSDATA/osdata.h"


namespace FS_STUFF
{
    char* GetDriveNameFromFullPath(const char* path)
    {
        if (path == NULL)
            return NULL;

        AddToStack();

        AddToStack();
        int fIndex = -1;
        int len = StrLen(path);
        RemoveFromStack();
        
        AddToStack();
        for (int i = 0; i < len; i++)
            if (path[i] == ':')
            {
                fIndex = i;
                break;
            }
        RemoveFromStack();

        if (fIndex < 1)
        {
            RemoveFromStack();
            return NULL;
        }
        
        int nLen = fIndex;
        char* name = (char*)malloc(nLen + 1, "drive name malloc");
        for (int i = 0; i < nLen; i++)
            name[i] = path[i];
        name[nLen] = 0;

        RemoveFromStack();
        return name;
    }

    char* GetFilePathFromFullPath(const char* path)
    {
        if (path == NULL)
            return NULL;

        AddToStack();

        int fIndex = -1;
        int len = StrLen(path);
        for (int i = 0; i < len; i++)
            if (path[i] == ':')
            {
                fIndex = i;
                break;
            }
        if (fIndex < 1)
        {
            RemoveFromStack();
            return NULL;
        }
        
        // AB :  XYZ (6)
        // 01 2 345
        // 6 - (2 + 1) => 3

        int nLen = len - (fIndex + 1);
        char* name = (char*)malloc(nLen + 1, "path name malloc");
        for (int i = 0; i < nLen; i++)
            name[i] = path[fIndex + 1 + i];
        name[nLen] = 0;

        RemoveFromStack();
        return name;
    }
    
    FilesystemInterface::GenericFilesystemInterface* GetFsInterfaceFromFullPath(const char* path)
    {
        if (path == NULL)
            return NULL;


        AddToStack();

        char* drive = GetDriveNameFromFullPath(path);
        if (drive == NULL)
        {
            RemoveFromStack();
            return NULL;
        }
        
        FilesystemInterface::GenericFilesystemInterface* foundFsInterface = NULL;
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
                    //Println(window, " - Drive: \"{}\"", partInfo->driveName, Colors.bgreen);
                    if (!StrEquals(partInfo->driveName, drive))
                        continue;

                    if (partInfo->fsInterface != NULL)
                    {
                        foundFsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInfo->fsInterface;
                        break;
                    }
                }
            }
        }
        
        if (foundFsInterface == NULL)
        {
            RemoveFromStack();
            return NULL;
        }

        RemoveFromStack();
        return foundFsInterface;
    }
    

    
}

