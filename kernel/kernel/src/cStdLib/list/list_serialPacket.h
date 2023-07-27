#pragma once
#include "../list.h"

#include "../../serialManager/serialManager.h"

template class Array<SerialManager::GenericPacket*>;
template class List<SerialManager::GenericPacket*>;

