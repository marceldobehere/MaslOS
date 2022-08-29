#pragma once
#include "fileSystemStructs.h"
//#include "../../../../memory/heap.h"
//#include "../../Partition_Interfaces/generic/genericPartitionInterface.h"
#include "../../Partition_Interfaces/generic/genericPartitionInterface.h"


namespace FilesystemInterface
{
    class GenericFilesystemInterface
    {
    private:

    public:
        PartitionInterface::GenericPartitionInterface* partitionInterface = NULL;
        PartitionInterface::PartitionInfo* partitionInfo = NULL;
        FilesystemInterfaceType InterfaceType = FilesystemInterface::Generic;
        GenericFilesystemInterface();
        //void* partitionInterface = NULL;
        //virtual ~GenericDiskInterface() {}
        

    };
}