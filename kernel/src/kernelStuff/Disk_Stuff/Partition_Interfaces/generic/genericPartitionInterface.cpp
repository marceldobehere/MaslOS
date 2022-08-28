#include "genericPartitionInterface.h"
#include "../mraps/mrapsPartitionInterface.h"

namespace PartitionInterface
{
    GenericPartitionInterface::GenericPartitionInterface()
    {

    }


    const char* GenericPartitionInterface::CreatePartition(PartitionInfo* partition, uint64_t size)
    {
        switch (interfaceType)
        {
            case PartitionInterfaceType::mraps:
            {
                return ((MRAPSPartitionInterface*)this)->CreatePartition(partition, size);
            }
        }

        return CommandResult.ERROR_GENERIC_INTERFACE;
    }
    const char* GenericPartitionInterface::ResizePartition(PartitionInfo* partition, uint64_t newSize)
    {
        switch (interfaceType)
        {
            case PartitionInterfaceType::mraps:
            {
                return ((MRAPSPartitionInterface*)this)->ResizePartition(partition, newSize);
            }
        }

        return CommandResult.ERROR_GENERIC_INTERFACE;
    }
    
    const char* GenericPartitionInterface::DeletePartition(PartitionInfo* partition)
    {
        switch (interfaceType)
        {
            case PartitionInterfaceType::mraps:
            {
                return ((MRAPSPartitionInterface*)this)->DeletePartition(partition);
            }
        }

        return CommandResult.ERROR_GENERIC_INTERFACE;
    }
    const char* GenericPartitionInterface::WipePartitionContents(PartitionInfo* partition)
    {
        switch (interfaceType)
        {
            case PartitionInterfaceType::mraps:
            {
                return ((MRAPSPartitionInterface*)this)->WipePartitionContents(partition);
            }
        }

        return CommandResult.ERROR_GENERIC_INTERFACE;
    }

    const char* GenericPartitionInterface::ReadPartition(PartitionInfo* partition, uint64_t address, uint64_t sizeInBytes, void* buffer)
    {
        switch (interfaceType)
        {
            case PartitionInterfaceType::mraps:
            {
                return ((MRAPSPartitionInterface*)this)->ReadPartition(partition, address, sizeInBytes, buffer);
            }
        }

        return CommandResult.ERROR_GENERIC_INTERFACE;
    }
    const char* GenericPartitionInterface::WritePartition(PartitionInfo* partition, uint64_t address, uint64_t sizeInBytes, void* buffer)
    {
        switch (interfaceType)
        {
            case PartitionInterfaceType::mraps:
            {
                return ((MRAPSPartitionInterface*)this)->WritePartition(partition, address, sizeInBytes, buffer);
            }
        }

        return CommandResult.ERROR_GENERIC_INTERFACE;
    }

    const char* GenericPartitionInterface::LoadPartitionTable()
    {
        switch (interfaceType)
        {
            case PartitionInterfaceType::mraps:
            {
                return ((MRAPSPartitionInterface*)this)->LoadPartitionTable();
            }
        }

        return CommandResult.ERROR_GENERIC_INTERFACE;
    }
    const char* GenericPartitionInterface::SavePartitionTable()
    {   
        switch (interfaceType)
        {
            case PartitionInterfaceType::mraps:
            {
                return ((MRAPSPartitionInterface*)this)->SavePartitionTable();
            }
        }

        return CommandResult.ERROR_GENERIC_INTERFACE;
    }
    
    const char* GenericPartitionInterface::InitAndSavePartitionTable()
    {   
        switch (interfaceType)
        {
            case PartitionInterfaceType::mraps:
            {
                return ((MRAPSPartitionInterface*)this)->InitAndSavePartitionTable();
            }
        }

        return CommandResult.ERROR_GENERIC_INTERFACE;
    }
}