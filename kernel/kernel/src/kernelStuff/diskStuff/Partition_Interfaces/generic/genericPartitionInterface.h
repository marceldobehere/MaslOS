#pragma once
#include <stdint.h>
#include <stddef.h>
#include "partitionStructs.h"
#include "../../../../cStdLib/list.h"


namespace PartitionInterface
{    

    PartitionInterfaceType GetPartitionInterfaceTypeFromDisk(DiskInterface::GenericDiskInterface* diskInterface);

    class GenericPartitionInterface
    {
    public:
        const uint16_t sizeOfPartitionInfoStruct = sizeof(PartitionInfo);
        bool busy = false;
        DiskInterface::GenericDiskInterface* diskInterface;
        List<PartitionInfo*> partitionList = List<PartitionInfo*>(10);
        PartitionInterfaceType interfaceType = PartitionInterfaceType::generic;
        GenericPartitionInterface();

        const char* CreatePartition(PartitionInfo* partition, uint64_t size);
        const char* ResizePartition(PartitionInfo* partition, uint64_t newSize);
        
        const char* DeletePartition(PartitionInfo* partition);
        const char* WipePartitionContents(PartitionInfo* partition);

        const char* ReadPartition (PartitionInfo* partition, uint64_t address, uint64_t sizeInBytes, void* buffer);
        const char* WritePartition(PartitionInfo* partition, uint64_t address, uint64_t sizeInBytes, void* buffer);

        const char* LoadPartitionTable();
        const char* InitAndSavePartitionTable();
        const char* SavePartitionTable();
    };
}