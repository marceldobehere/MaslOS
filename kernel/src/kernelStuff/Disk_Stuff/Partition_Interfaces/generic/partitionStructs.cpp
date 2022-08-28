#include "partitionStructs.h"
#include "../../../../cmdParsing/cstrTools.h"

namespace PartitionInterface
{
    _CommandResult CommandResult;

    PartitionInfo::PartitionInfo()
    {

    }
    
    PartitionInfo::PartitionInfo(const char* name, const char* description, const char* driveName, PartitionType type, uint64_t sizeInBytes, uint64_t locationInBytes, bool hidden, void* owner)
    {
        uint16_t nameLen = StrLen(name);
        uint16_t descLen = StrLen(description);
        uint16_t driveNameLen = StrLen(driveName);
        
        this->name = name;
        this->nameLen = nameLen;
        this->description = description;
        this->descriptionLen = descLen;
        this->driveName = driveName;
        this->driveNameLen = driveNameLen;
        this->type = type;
        this->sizeInBytes = sizeInBytes;
        this->locationInBytes = locationInBytes;
        this->owner = owner;
        this->hidden = hidden;
    }
}