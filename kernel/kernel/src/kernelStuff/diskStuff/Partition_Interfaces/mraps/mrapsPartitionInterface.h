#pragma once
#include "../generic/genericPartitionInterface.h"

namespace PartitionInterface
{
    // MRAPS -> Marcels Really Awesome Partitioning System
    class MRAPSPartitionInterface : public GenericPartitionInterface
    {
    public:
        MRAPSPartitionInterface(DiskInterface::GenericDiskInterface* diskInterface);
        uint64_t driveSize;

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