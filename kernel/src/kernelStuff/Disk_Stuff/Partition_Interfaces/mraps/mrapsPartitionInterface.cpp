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
    }
    // This partition interface reserves 20 sectors
    // (1 Boot sector + 19 Data sectors)

    const char* MRAPSPartitionInterface::InitAndSavePartitionTable()
    {
        if (driveSize < 20 * 512)
            return CommandResult.ERROR_DISK_TOO_SMALL;
        
        partitionList.clear();

        partitionList.add(new PartitionInfo("Boot Partition", "This is the Bootsector.", "boot_sector", PartitionType::Boot, 512, 0, true, (void*)this));
        partitionList.add(new PartitionInfo("Partition Table Partition", "This is the Partition Table which stores all the partition data.", "partition_manager", PartitionType::PartitionData, 512 * 19, 512, true, (void*)this));
        partitionList.add(new PartitionInfo("Empty Partition", "This is the default generated Partition made my MRAPS.", "empty_test", PartitionType::Undefined, (driveSize - (512 * 20)), 20 * 512, false, (void*)this));

        return CommandResult.SUCCESS;
    }


    const char* MRAPSPartitionInterface::CreatePartition(PartitionInfo* partition, uint64_t size)
    {




        return CommandResult.ERROR_NOT_IMPLEMENTED;
    }

    const char* MRAPSPartitionInterface::ResizePartition(PartitionInfo* partition, uint64_t newSize)
    {




        return CommandResult.ERROR_NOT_IMPLEMENTED;
    }

    
    const char* MRAPSPartitionInterface::DeletePartition(PartitionInfo* partition)
    {




        return CommandResult.ERROR_NOT_IMPLEMENTED;
    }

    const char* MRAPSPartitionInterface::WipePartitionContents(PartitionInfo* partition)
    {




        return CommandResult.ERROR_NOT_IMPLEMENTED;
    }

    const char* MRAPSPartitionInterface::ReadPartition (PartitionInfo* partition, uint64_t address, uint64_t sizeInBytes, void* buffer)
    {




        return CommandResult.ERROR_NOT_IMPLEMENTED;
    }

    const char* MRAPSPartitionInterface::WritePartition(PartitionInfo* partition, uint64_t address, uint64_t sizeInBytes, void* buffer)
    {




        return CommandResult.ERROR_NOT_IMPLEMENTED;
    }


    const char* MRAPSPartitionInterface::LoadPartitionTable()
    {
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