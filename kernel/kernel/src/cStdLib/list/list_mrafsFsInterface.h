#pragma once
#include "../list.h"

#include "../../kernelStuff/diskStuff/Filesystem_Interfaces/mrafs/mrafsFileSystemInterface.h"
template class Array<FilesystemInterface::MrafsFilesystemStuff::FSPartitionInfo*>;
template class  List<FilesystemInterface::MrafsFilesystemStuff::FSPartitionInfo*>;