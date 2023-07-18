#pragma once
#include "../list.h"

#include "../../WindowStuff/SubInstances/newTerminalInstance/consoleChar.h"

template class Array<ConsoleChar>;
template class List<ConsoleChar>;
template class Array<List<ConsoleChar>*>;
template class List<List<ConsoleChar>*>;