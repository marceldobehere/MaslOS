#pragma once

#include "../kernelStuff/diskStuff/Disk_Interfaces/ram/ramDiskInterface.h"
#include "../kernelStuff/diskStuff/Disk_Interfaces/file/fileDiskInterface.h"
#include "../kernelStuff/diskStuff/Partition_Interfaces/mraps/mrapsPartitionInterface.h"
#include "../kernelStuff/diskStuff/Filesystem_Interfaces/mrafs/mrafsFileSystemInterface.h"
#include "../tasks/readFile/taskReadFile.h"
#include "../tasks/writeFile/taskWriteFile.h"

namespace FS_STUFF
{
    struct FileBuffer
    {
        char* buffer;
        uint64_t bufferLen;
    };

    char* GetDriveNameFromFullPath(const char* path);
    char* GetFilePathFromFullPath(const char* path);
    char* GetFolderPathFromFullPath(const char* path);

    bool CreateFileIfNotExist(const char* path);
    bool FileExists(const char* path);

    bool ReadFileFromFullPath(const char* path, char** resBuffer, int* resBufferLen);
    bool WriteFileToFullPath(const char* path, char* buffer, int bufferLen, bool createIfNotExists);

    TaskReadFile* ReadFileTask(const char* path);
    TaskWriteFile* WriteFileTask(const char* path, char* buffer, int bufferLen, bool createIfNotExists);

    FilesystemInterface::GenericFilesystemInterface* GetFsInterfaceFromFullPath(const char* path);
    FilesystemInterface::FileInfo* GetFileInfoFromFullPath(const char* path);

    bool OpenFile(const char* path);
    void RunTerminalCommand(const char* terminalCommand, const char* terminalWindowTitle, bool hideTerminalWindow, bool autoCloseTerminalWindow);

    

}
