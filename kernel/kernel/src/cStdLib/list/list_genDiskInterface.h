#pragma once
#include "../list.h"


#include "../../kernelStuff/diskStuff/Disk_Interfaces/generic/genericDiskInterface.h"

template class Array<DiskInterface::GenericDiskInterface*>;
template class List<DiskInterface::GenericDiskInterface*>;