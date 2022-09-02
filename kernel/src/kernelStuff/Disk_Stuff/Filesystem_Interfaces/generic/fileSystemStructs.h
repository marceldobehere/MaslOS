#pragma once
#include <stdint.h>
#include <stddef.h>

namespace FilesystemInterface
{
    enum FilesystemInterfaceType : uint8_t
    {
        None = 0,
        Generic = 1,
        Mrafs = 2
    };

    extern const char* FilesystemInterfaceTypeStr[];

    struct _FSCommandResult
    {
        const char* SUCCESS = "SUCCESS";
        const char* ERROR_DEFAULT = "ERROR";
        const char* ERROR_GENERIC_INTERFACE = "ERROR: THE INTERFACE IS GENERIC";
        const char* ERROR_FUNCTION_NOT_IMPLEMENTED = "ERROR: THE FUNCTION WAS NOT IMPLEMENTED";
        const char* ERROR_PARTITION_TOO_SMALL = "ERROR: PARTITION TOO SMALL";
        const char* ERROR_PARTITION_NOT_FOUND = "ERROR: PARTITION NOT FOUND";
        const char* ERROR_NO_PARTITION = "ERROR: THE FILESYSTEM IS NOT ATTACHED TO A PARTITION";
        const char* ERROR_NO_PARTITION_INTERFACE = "ERROR: THE FILESYSTEM IS NOT ATTACHED TO A PARTITION INTERFACE";
        const char* ERROR_INVALID_PARTITION_OWNER = "ERROR: INVALID PARTITION OWNER";
        const char* ERROR_FS_TABLE_TOO_BIG = "ERROR: FILESYSTEM TABLE TOO BIG";
        const char* ERROR_FS_BUFFER_TOO_BIG = "ERROR: BUFFER TOO BIG FOR FILE";
        const char* ERROR_FILE_ALREADY_EXISTS = "ERROR: FILE ALREADY EXISTS";
        const char* ERROR_FOLDER_ALREADY_EXISTS = "ERROR: FOLDER ALREADY EXISTS";
        const char* ERROR_FILE_NOT_FOUND = "ERROR: FILE NOT FOUND";
        const char* ERROR_FOLDER_NOT_FOUND = "ERROR: FOLDER NOT FOUND";
        const char* ERROR_PATH_IS_THE_SAME = "ERROR: PATH IS THE SAME";
        const char* ERROR_FILE_TOO_SMALL = "ERROR: FILE TOO SMALL";
    };

    extern _FSCommandResult FSCommandResult;

    struct BaseInfo
    {
        const char* path;
        uint16_t pathLen;
        bool writeProtected;
        bool hidden;
        bool systemFile;
        
        BaseInfo(const char* path, bool writeProtected, bool hidden, bool systemFile);
        BaseInfo();
    };

    struct FileInfo
    {
        BaseInfo baseInfo;

        uint64_t sizeInBytes;
        uint64_t locationInBytes;

        FileInfo(BaseInfo baseInfo, uint64_t sizeInBytes, uint64_t locationInBytes);
    };

    struct FolderInfo
    {
        BaseInfo baseInfo;

        FolderInfo(BaseInfo baseInfo);
    };
}