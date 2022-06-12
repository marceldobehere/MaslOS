#pragma once
#include "../../../OSDATA/userdata.h"
#include "../defaultInstance/defaultInstance.h"

class TerminalInstance : public DefaultInstance
{
    private:


    public:
    OSUser* currentUser;
    char lastTerminalInput[256];
    char terminalInput[256];
    uint64_t userlen;
    TerminalInstance(OSUser* user);

};