#include "taskReadFile.h"
#include "../../memory/heap.h"
#include "../../cStdLib/cstrTools.h"

#include "../../kernelStuff/diskStuff/Disk_Interfaces/ram/ramDiskInterface.h"
#include "../../kernelStuff/diskStuff/Disk_Interfaces/file/fileDiskInterface.h"
#include "../../kernelStuff/diskStuff/Partition_Interfaces/mraps/mrapsPartitionInterface.h"
#include "../../kernelStuff/diskStuff/Filesystem_Interfaces/mrafs/mrafsFileSystemInterface.h"
#include "../../fsStuff/fsStuff.h"
#include "../..//devices/serial/serial.h"
#include "../../cStdLib/cstr.h"

TaskReadFile::TaskReadFile(const char* path)
{
    done = false;
    type = TaskType::READ_FILE;
    data = NULL;
    dataLen = 0;
    dataPos = 0;
    this->path = StrCopy(path);
}


void TaskReadFile::SetDone()
{
    done = true;
}
void TaskReadFile::Do()
{
    if (dataPos >= dataLen && dataLen > 0)
    {
        SetDone();
        return;
    }

    FilesystemInterface::GenericFilesystemInterface* fsInterface = FS_STUFF::GetFsInterfaceFromFullPath(path);
    if (fsInterface == NULL)
    {
        SetDone();
        return;
    }
    
    char* relPath = FS_STUFF::GetFilePathFromFullPath(path);
    if (relPath == NULL)
    {
        SetDone();
        return;
    }

    if (!fsInterface->FileExists(relPath))
    {
        _Free(relPath);
        {
            SetDone();
            return;
        }
    }

    if (data == NULL)
    {
        dataLen = fsInterface->GetFileInfo(relPath)->sizeInBytes;
        data = (char*)_Malloc(dataLen, "Read File Task Data Buffer");
        dataPos = 0;
    }

    int64_t dataLeft = (dataLen - dataPos);
    int64_t readLen = min(dataLeft, MaxBufferStep);

    fsInterface->ReadFileBuffer(relPath, dataPos, readLen, (void*)(data + dataPos));
    dataPos += readLen;
    //Serial::Writeln("{}% done", to_string((dataPos * 100) / dataLen));

    _Free(relPath);


    if (dataPos >= dataLen)
    {
        SetDone();
        return;
    }
}

void TaskReadFile::Free()
{
    // if (data != NULL)
    // {
    //     _Free(data);
    //     data = NULL;
    // }
    _Free(path);
}

TaskReadFile* NewReadFileTask(const char* path)
{
    TaskReadFile* task = (TaskReadFile*)_Malloc(sizeof(TaskReadFile), "New Read File Task");
    *task = TaskReadFile(path);
    return task;
}
