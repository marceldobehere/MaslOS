#pragma once
#include "../queue.h"

#include "../../serialManager/serialManager.h"

template class Queue<SerialManager::GenericPacket*>;