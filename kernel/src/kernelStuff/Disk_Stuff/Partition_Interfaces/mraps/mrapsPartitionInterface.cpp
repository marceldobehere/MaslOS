#include "mrapsPartitionInterface.h"

#include "../../../../customClasses/list.h"
#include "../../../../memory/heap.h"
#include "../../../../OSDATA/osdata.h"

namespace PartitionInterface
{
    MRAPSPartitionInterface::MRAPSPartitionInterface(DiskInterface::GenericDiskInterface* diskInterface)
    {
        this->interfaceType = PartitionInterfaceType::mraps;
        this->diskInterface = diskInterface;
        diskInterface->partitionInterface = (void*)this;
        sizeof(PartitionInfo);
        busy = false;
        driveSize = diskInterface->GetMaxSectorCount() * 512;
        partitionList.clear();
    }
    // This partition interface reserves 20 sectors
    // (1 Boot sector + 19 Data sectors)

    const char* MRAPSPartitionInterface::InitAndSavePartitionTable()
    {
        if (driveSize < 20 * 512)
            return CommandResult.ERROR_DISK_TOO_SMALL;
        
        for (int i = 0; i < partitionList.getCount(); i++)
            partitionList[i]->Destroy();
        partitionList.clear();

        partitionList.add(new PartitionInfo("Boot Partition", "This is the Bootsector.", "boot_sector", PartitionType::Boot, 512, 0, true, (void*)this));
        partitionList.add(new PartitionInfo("Partition Table Partition", "This is the Partition Table which stores all the partition data.", "partition_manager", PartitionType::PartitionData, 512 * 19, 512, true, (void*)this));
        partitionList.add(new PartitionInfo("Empty Partition", "This is the default generated Partition made my MRAPS.", "empty_test", PartitionType::Undefined, (driveSize - (512 * 20)), 20 * 512, false, (void*)this));

        return SavePartitionTable();
    }


    const char* MRAPSPartitionInterface::CreatePartition(PartitionInfo* partition, uint64_t size)
    {
        if ((uint64_t)partition->owner != (uint64_t)this)
            return CommandResult.ERROR_INVALID_PARTITION_OWNER;
        if (size > partition->sizeInBytes)
            return CommandResult.ERROR_PARTITION_TOO_SMALL;
        if (partition->type != PartitionType::Undefined)
            return CommandResult.ERROR_PARTITION_NOT_UNDEFINED;

        partition->type = PartitionType::Normal;
        if (size < partition->sizeInBytes)
            return ResizePartition(partition, size);
        
        return CommandResult.SUCCESS;
    }

    const char* MRAPSPartitionInterface::ResizePartition(PartitionInfo* partition, uint64_t newSize)
    {
        if ((uint64_t)partition->owner != (uint64_t)this)
            return CommandResult.ERROR_INVALID_PARTITION_OWNER;
        if (partition->type == PartitionType::Boot)
            return CommandResult.ERROR_CANNOT_EDIT_BOOT_PARTITION;
        if (partition->type == PartitionType::Reserved)
            return CommandResult.ERROR_CANNOT_EDIT_RESERVED_PARTITION;
        if (partition->type == PartitionType::PartitionData)
            return CommandResult.ERROR_CANNOT_EDIT_TABLE_PARTITION;
        if (newSize == partition->sizeInBytes)
            return CommandResult.SUCCESS;

        if (newSize > partition->sizeInBytes)
        {
            int indexOfNextPartition = partitionList.getIndexOf(partition) + 1;
            if (indexOfNextPartition == 0 || indexOfNextPartition >= partitionList.getCount())
                return CommandResult.ERROR_PARTITION_TOO_SMALL;
            PartitionInfo* nextPartition = partitionList[indexOfNextPartition];
            if (nextPartition->type != PartitionType::Undefined)
                return CommandResult.ERROR_NO_FREE_PARTITION_AFTER;
            if ((newSize - partition->sizeInBytes) > nextPartition->sizeInBytes)
                return CommandResult.ERROR_PARTITION_AFTER_TOO_SMALL;
            
            if ((newSize - partition->sizeInBytes) == nextPartition->sizeInBytes)
            {
                // INCREASE THIS PARTITIONS SIZE BY THE SIZE OF NEXT PARTITION
                partition->sizeInBytes += nextPartition->sizeInBytes;
                partitionList.removeAt(indexOfNextPartition);
                nextPartition->Destroy();
            }
            else
            {
                // INCREASE THIS PARTITION SIZE BY THE AMOUNT NEEDED
                uint64_t ogSize = partition->sizeInBytes;
                uint64_t remainingSize = newSize - ogSize;
                partition->sizeInBytes = newSize;
                // DECREASE NEXT PARTITIONS SIZE
                nextPartition->sizeInBytes -= remainingSize;
                // MOVE NEXT PARTITION LOCATION
                nextPartition->locationInBytes += remainingSize;
            }

            return CommandResult.SUCCESS;
        }

        {
            uint64_t ogSize = partition->sizeInBytes;
            uint64_t remainingSize = ogSize - newSize;
            partition->sizeInBytes = newSize;

            int indexOfCurrentPartition = partitionList.getIndexOf(partition);
            if (indexOfCurrentPartition == -1)
                return CommandResult.ERROR_PARTITION_NOT_FOUND;
            
            PartitionInfo* newPartition = new PartitionInfo();
            newPartition->sizeInBytes = remainingSize;
            newPartition->hidden = false;
            newPartition->locationInBytes = partition->locationInBytes + partition->sizeInBytes;
            newPartition->owner = this;
            newPartition->type = PartitionType::Undefined;
            partitionList.insertAt(newPartition, indexOfCurrentPartition + 1);
            return DeletePartition(newPartition); // To make it combine with other empty partitions if they exist
        }

        return CommandResult.SUCCESS;
    }

    
    const char* MRAPSPartitionInterface::DeletePartition(PartitionInfo* partition)
    {
        if ((uint64_t)partition->owner != (uint64_t)this)
            return CommandResult.ERROR_INVALID_PARTITION_OWNER;
        if (partition->type == PartitionType::Boot)
            return CommandResult.ERROR_CANNOT_EDIT_BOOT_PARTITION;
        if (partition->type == PartitionType::Reserved)
            return CommandResult.ERROR_CANNOT_EDIT_RESERVED_PARTITION;
        if (partition->type == PartitionType::PartitionData)
            return CommandResult.ERROR_CANNOT_EDIT_TABLE_PARTITION;

        // Set Partition Type To Undefined
        partition->type = PartitionType::Undefined;

        int indexOfThisPartition = partitionList.getIndexOf(partition);
        if (indexOfThisPartition == -1)
            return CommandResult.ERROR_PARTITION_NOT_FOUND;
        
        // Check if Partition After is undefined aswell (combine if yes)
        if ((indexOfThisPartition + 1) < partitionList.getCount())
        {
            PartitionInfo* nextPart = partitionList[indexOfThisPartition + 1];
            if (nextPart->type == PartitionType::Undefined)
            {
                const char* res = ResizePartition(partition, partition->sizeInBytes + nextPart->sizeInBytes);
                if (res != CommandResult.SUCCESS)
                    return res;
            }
        }

        // Check if Partition Before is undefined aswell (combine if yes)
        if ((indexOfThisPartition - 1) >= 0)
        {
            PartitionInfo* prevPart = partitionList[indexOfThisPartition - 1];
            if (prevPart->type == PartitionType::Undefined)
            {
                const char* res = ResizePartition(prevPart, partition->sizeInBytes + prevPart->sizeInBytes);
                if (res != CommandResult.SUCCESS)
                    return res;
            }
        }

        return CommandResult.SUCCESS;
    }

    const char* MRAPSPartitionInterface::WipePartitionContents(PartitionInfo* partition)
    {
        if ((uint64_t)partition->owner != (uint64_t)this)
            return CommandResult.ERROR_INVALID_PARTITION_OWNER;
        if (partition->type == PartitionType::Reserved)
            return CommandResult.ERROR_CANNOT_EDIT_RESERVED_PARTITION;
        if (partition->type == PartitionType::PartitionData)
            return CommandResult.ERROR_CANNOT_EDIT_TABLE_PARTITION;
        
        
        uint64_t splitSize = 5000000;
        uint64_t size = partition->sizeInBytes;
        if (size <= splitSize)
        {
            void* buffer = malloc(size, "Wipe Partition Buffer");
            _memset(buffer, 0, size);
            if(!diskInterface->WriteBytes(partition->locationInBytes, size, buffer))
            {
                free(buffer);
                return CommandResult.ERROR_WRITE_FAILED;
            }
            free(buffer);
        }
        else
        {
            void* buffer = malloc(splitSize, "Wipe Partition Buffer");
            _memset(buffer, 0, splitSize);

            uint64_t i = 0;
            uint64_t splitCount = size / splitSize; 
            for (; i < splitCount; i++)
                if(!diskInterface->WriteBytes(partition->locationInBytes + i * splitSize, splitSize, buffer))
                {
                    free(buffer);
                    return CommandResult.ERROR_WRITE_FAILED;
                }
            uint64_t remainder = (size % splitSize);
            if (remainder != 0)
                if(!diskInterface->WriteBytes(partition->locationInBytes + i * splitSize, remainder, buffer))
                {
                    free(buffer);
                    return CommandResult.ERROR_WRITE_FAILED;
                }

            free(buffer);
        }

        return CommandResult.SUCCESS;
    }

    const char* MRAPSPartitionInterface::ReadPartition (PartitionInfo* partition, uint64_t address, uint64_t sizeInBytes, void* buffer)
    {
        if ((uint64_t)partition->owner != (uint64_t)this)
            return CommandResult.ERROR_INVALID_PARTITION_OWNER;
        if (address + sizeInBytes >= partition->sizeInBytes)
            return CommandResult.ERROR_PARTITION_TOO_SMALL;
        if (sizeInBytes == 0)
            return CommandResult.SUCCESS;
        if(!diskInterface->ReadBytes(partition->locationInBytes + address, sizeInBytes, buffer))
            return CommandResult.ERROR_READ_FAILED;

        return CommandResult.SUCCESS;
    }

    const char* MRAPSPartitionInterface::WritePartition(PartitionInfo* partition, uint64_t address, uint64_t sizeInBytes, void* buffer)
    {
        if ((uint64_t)partition->owner != (uint64_t)this)
            return CommandResult.ERROR_INVALID_PARTITION_OWNER;
        if (partition->type == PartitionType::Reserved)
            return CommandResult.ERROR_CANNOT_EDIT_RESERVED_PARTITION;
        if (partition->type == PartitionType::PartitionData)
            return CommandResult.ERROR_CANNOT_EDIT_TABLE_PARTITION;
        if (address + sizeInBytes >= partition->sizeInBytes)
            return CommandResult.ERROR_PARTITION_TOO_SMALL;
        if (sizeInBytes == 0)
            return CommandResult.SUCCESS;
        if(!diskInterface->WriteBytes(partition->locationInBytes + address, sizeInBytes, buffer))
            return CommandResult.ERROR_WRITE_FAILED;

        return CommandResult.SUCCESS;
    }


    const char* MRAPSPartitionInterface::LoadPartitionTable()
    {
        for (int i = 0; i < partitionList.getCount(); i++)
            partitionList[i]->Destroy();
        partitionList.clear();
        uint8_t* buffer = (uint8_t*)malloc(19*512, "Malloc For Buffer for Disk Read");
        _memset(buffer, 0, 19 * 512);
        
        if (!diskInterface->Read(1, 19, buffer))
            return CommandResult.ERROR_READ_FAILED;

        uint64_t totalSize = 0;
        uint16_t partitionCount = 0;
        uint8_t* tempBuffer = buffer;
        {
            totalSize = *((uint64_t*)tempBuffer);
            tempBuffer += 8;

            if (totalSize >= 19 * 512)
                return CommandResult.ERROR_DISK_TOO_SMALL;

            partitionCount = *((uint16_t*)tempBuffer);
            tempBuffer += 2;

            for (int i = 0; i < partitionCount; i++)
            {
                PartitionInfo* info = new PartitionInfo();

                {
                    free((void*)info->name);
                    info->nameLen = *((uint16_t*)tempBuffer);
                    char* tempStr = (char*)malloc(info->nameLen + 1);
                    tempStr[info->nameLen] = 0;
                    info->name = (const char*)tempStr;
                    tempBuffer += 2;
                    for (int x = 0; x < info->nameLen; x++)
                    {
                        tempStr[x] = *tempBuffer;
                        tempBuffer++;
                    }
                }

                {
                    free((void*)info->description);
                    info->descriptionLen = *((uint16_t*)tempBuffer);
                    char* tempStr = (char*)malloc(info->descriptionLen + 1);
                    tempStr[info->descriptionLen] = 0;
                    info->description = (const char*)tempStr;
                    tempBuffer += 2;
                    for (int x = 0; x < info->descriptionLen; x++)
                    {
                        tempStr[x] = *tempBuffer;
                        tempBuffer++;
                    }
                }

                {
                    free((void*)info->driveName);
                    info->driveNameLen = *((uint16_t*)tempBuffer);
                    char* tempStr = (char*)malloc(info->driveNameLen + 1);
                    tempStr[info->driveNameLen] = 0;
                    info->driveName= (const char*)tempStr;
                    tempBuffer += 2;
                    for (int x = 0; x < info->driveNameLen; x++)
                    {
                        tempStr[x] = *tempBuffer;
                        tempBuffer++;
                    }
                }

                info->hidden = *((uint8_t*)tempBuffer);
                tempBuffer += 1;

                info->sizeInBytes = *((uint64_t*)tempBuffer);
                tempBuffer += 8;

                info->locationInBytes = *((uint64_t*)tempBuffer);
                tempBuffer += 8;

                info->type = (PartitionType)*((uint8_t*)tempBuffer);
                tempBuffer += 1;

                info->owner = (void*)this;

                partitionList.add(info);
            }
        }

        _free(buffer);
        return CommandResult.SUCCESS;
    }

    const char* MRAPSPartitionInterface::SavePartitionTable()
    {
        // Sector 1 - 19
        uint16_t partitionCount = (uint16_t)partitionList.getCount();
        uint64_t totalSize = sizeof(totalSize) + sizeof(partitionCount);
        {
            for (int i = 0; i < partitionCount; i++)
            {
                PartitionInfo* info = partitionList[i];
                totalSize += info->nameLen        + sizeof(info->nameLen);
                totalSize += info->descriptionLen + sizeof(info->descriptionLen);
                totalSize += info->driveNameLen   + sizeof(info->driveNameLen);
                totalSize += sizeof(info->hidden);
                totalSize += sizeof(info->sizeInBytes);
                totalSize += sizeof(info->locationInBytes);
                totalSize += sizeof(info->type);
            }
        }

        if (totalSize >= 19 * 512)
            return CommandResult.ERROR_DISK_TOO_SMALL;

        //osData.mainTerminalWindow->Log("Total Size: {} Bytes", to_string(totalSize), Colors.bgreen);

        uint8_t* buffer = (uint8_t*)malloc(19*512, "Malloc For Buffer for Disk Write");
        _memset(buffer, 0, 19 * 512);
        uint8_t* tempBuffer = buffer;
        {
            *((uint64_t*)tempBuffer) = totalSize;
            tempBuffer += 8;

            *((uint16_t*)tempBuffer) = partitionCount;
            tempBuffer += 2;

            for (int i = 0; i < partitionCount; i++)
            {
                PartitionInfo* info = partitionList[i];

                *((uint16_t*)tempBuffer) = info->nameLen;
                tempBuffer += 2;
                for (int x = 0; x < info->nameLen; x++)
                {
                    *tempBuffer = info->name[x];
                    tempBuffer++;
                }

                *((uint16_t*)tempBuffer) = info->descriptionLen;
                tempBuffer += 2;
                for (int x = 0; x < info->descriptionLen; x++)
                {
                    *tempBuffer = info->description[x];
                    tempBuffer++;
                }

                *((uint16_t*)tempBuffer) = info->driveNameLen;
                tempBuffer += 2;
                for (int x = 0; x < info->driveNameLen; x++)
                {
                    *tempBuffer = info->driveName[x];
                    tempBuffer++;
                }

                *((uint8_t*)tempBuffer) = info->hidden;
                tempBuffer += 1;

                *((uint64_t*)tempBuffer) = info->sizeInBytes;
                tempBuffer += 8;

                *((uint64_t*)tempBuffer) = info->locationInBytes;
                tempBuffer += 8;

                *((uint8_t*)tempBuffer) = info->type;
                tempBuffer += 1;
            }
        }
        
        //osData.mainTerminalWindow->Log("First Buffer   Address: 0x{}", ConvertHexToString((uint64_t)buffer), Colors.bgreen);
        //osData.mainTerminalWindow->Log("Last Buffer    Address: 0x{}", ConvertHexToString((uint64_t)tempBuffer), Colors.bgreen);
        //osData.mainTerminalWindow->Log("Disk Interface Address: 0x{}", ConvertHexToString((uint64_t)diskInterface), Colors.bgreen);

        if (!diskInterface->Write(1, 19, buffer))
            return CommandResult.ERROR_WRITE_FAILED;

        _free(buffer);
        return CommandResult.SUCCESS;
    }
}