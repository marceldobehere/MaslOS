#pragma once
#include "../../../OSDATA/userdata.h"
#include "../defaultInstance/defaultInstance.h"

class TerminalInstance : public DefaultInstance
{
    private:


    public:
    OSUser* currentUser;
    char terminalInput[256];
    TerminalInstance(OSUser* user);

};