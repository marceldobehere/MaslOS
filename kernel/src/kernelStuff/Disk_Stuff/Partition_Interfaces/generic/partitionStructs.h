#pragma once
#include <stdint.h>

namespace PartitionInterface
{
    enum PartitionInterfaceType : uint8_t
    {
        generic = 0,
        mraps = 1 
    };

    enum PartitionType : uint8_t
    {
        Undefined = 0,
        Empty = 1,
        Normal = 2,
        Reserved = 3,
        Boot = 4,
        PartitionData = 5
    };

    struct _CommandResult
    {
        const char* SUCCESS = "SUCCESS";
        const char* ERROR_DEFAULT = "ERROR";
        const char* ERROR_GENERIC_INTERFACE = "ERROR: THE INTERFACE IS GENERIC";
        const char* ERROR_INDEX_OUT_OF_RANGE = "ERROR: THE INDEX IS OUT OF RANGE";
        const char* ERROR_NOT_IMPLEMENTED = "ERROR: FUNCTION NOT IMPLEMENTED";
        const char* ERROR_DISK_TOO_SMALL = "ERROR: DISK TOO SMALL";
        const char* ERROR_INVALID_PARTITION_OWNER = "ERROR: INVALID PARTITION OWNER";
        const char* ERROR_READ_FAILED = "ERROR: READ FAILED";
        const char* ERROR_WRITE_FAILED = "ERROR: WRITE FAILED";
    };
    extern _CommandResult CommandResult;


    struct PartitionInfo
    {
        bool hidden;
        uint16_t nameLen;
        const char* name;
        uint16_t descriptionLen;
        const char* description;
        uint16_t driveNameLen;
        const char* driveName;

        PartitionType type;
        uint64_t sizeInBytes;
        uint64_t locationInBytes;
        void* owner;
        PartitionInfo();
        PartitionInfo(const char* name, const char* description, const char* driveName, PartitionType type, uint64_t sizeInBytes, uint64_t locationInBytes, bool hidden, void* owner);
    };
}