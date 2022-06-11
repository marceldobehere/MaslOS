#include "terminalInstance.h"

TerminalInstance::TerminalInstance(OSUser* user)
{
    this->currentUser = user;
    instanceType = InstanceType::Terminal;
}