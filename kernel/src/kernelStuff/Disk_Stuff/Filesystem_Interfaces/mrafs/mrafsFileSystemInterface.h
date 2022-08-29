#pragma once
#include "../generic/genericFileSystemInterface.h"

namespace FilesystemInterface
{
    class MrafsFilesystemInterface : public GenericFilesystemInterface
    {
    private:

    public:
        FilesystemInterfaceType InterfaceType = FilesystemInterface::Mrafs;
        //void* partitionInterface = NULL;
        MrafsFilesystemInterface(PartitionInterface::GenericPartitionInterface* partitionInterface, PartitionInterface::PartitionInfo* partitionInfo);
        

    };
}