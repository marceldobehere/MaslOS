#include "partitionStructs.h"
#include "../../../../cmdParsing/cstrTools.h"
#include "../../../../memory/heap.h"

namespace PartitionInterface
{
    _CommandResult CommandResult;

    PartitionInfo::PartitionInfo()
    {
        this->name = StrCopy("");
        this->nameLen = 0;
        this->description = StrCopy("");
        this->descriptionLen = 0;
        this->driveName = StrCopy("");
        this->driveNameLen = 0;
        this->type = PartitionType::Undefined;
        this->sizeInBytes = 0;
        this->locationInBytes = 0;
        this->owner = 0;
        this->hidden = false;
    }

    void PartitionInfo::PartitionInfo::Destroy()
    {
        free((void*)name);
        free((void*)description);
        free((void*)driveName);
        free((void*)this);
    }
    
    PartitionInfo::PartitionInfo(const char* name, const char* description, const char* driveName, PartitionType type, uint64_t sizeInBytes, uint64_t locationInBytes, bool hidden, void* owner)
    {
        uint16_t nameLen = StrLen(name);
        uint16_t descLen = StrLen(description);
        uint16_t driveNameLen = StrLen(driveName);
        
        this->name = StrCopy(name);
        this->nameLen = nameLen;
        this->description = StrCopy(description);
        this->descriptionLen = descLen;
        this->driveName = StrCopy(driveName);
        this->driveNameLen = driveNameLen;
        this->type = type;
        this->sizeInBytes = sizeInBytes;
        this->locationInBytes = locationInBytes;
        this->owner = owner;
        this->hidden = hidden;
    }
}