#include "taskWriteFile.h"
#include "../../memory/heap.h"
#include "../../cStdLib/cstrTools.h"

#include "../../kernelStuff/diskStuff/Disk_Interfaces/ram/ramDiskInterface.h"
#include "../../kernelStuff/diskStuff/Disk_Interfaces/file/fileDiskInterface.h"
#include "../../kernelStuff/diskStuff/Partition_Interfaces/mraps/mrapsPartitionInterface.h"
#include "../../kernelStuff/diskStuff/Filesystem_Interfaces/mrafs/mrafsFileSystemInterface.h"
#include "../../fsStuff/fsStuff.h"

TaskWriteFile::TaskWriteFile(const char* path, char* buffer, uint64_t len)
{
    done = false;
    type = TaskType::WRITE_FILE;
    data = buffer;
    dataLen = len;
    dataPos = 0;
    this->path = StrCopy(path);

    this->TaskText = "<WRITE FILE TASK>";
    this->DoTaskFuncHelp = (void*)this;
    this->DoTaskFunc = (void(*)(void*))(void*)&Do;
    this->FreeTaskFuncHelp = (void*)this;
    this->FreeTaskFunc = (void(*)(void*))(void*)&Free;
}


void TaskWriteFile::SetDone()
{
    done = true;
}
void TaskWriteFile::Do()
{
    if (dataPos >= dataLen)
    {
        SetDone();
        return;
    }

    if (data == NULL)
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

    // If Filesize doesn't match, delete File and Create File with matching size
    {
        FilesystemInterface::FileInfo* info = fsInterface->GetFileInfo(relPath);
        if (info->sizeInBytes != dataLen)
        {
            fsInterface->DeleteFile(relPath);
            fsInterface->CreateFile(relPath, dataLen);
            dataPos = 0;
        }
        info->Destroy();
    }

    int64_t dataLeft = (dataLen - dataPos);
    int64_t readLen = min(dataLeft, MaxBufferStep);

    fsInterface->WriteFileBuffer(relPath, dataPos, readLen, (void*)(data + dataPos));
    dataPos += readLen;

    _Free(relPath);


    if (dataPos >= dataLen)
    {
        SetDone();
        return;
    }
}

void TaskWriteFile::Free()
{
    _Free(path);
}

TaskWriteFile* NewWriteFileTask(const char* path, char* buffer, uint64_t len)
{
    TaskWriteFile* task = new TaskWriteFile(path, buffer, len);
    return task;
}
