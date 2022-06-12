#include "terminalInstance.h"

TerminalInstance::TerminalInstance(OSUser* user)
{
    this->currentUser = user;
    instanceType = InstanceType::Terminal;
    userlen = 0;
    for (int i = 0; i < 256; i++)
    {
        terminalInput[i] = 0;
        lastTerminalInput[i] = 0;
    }
}