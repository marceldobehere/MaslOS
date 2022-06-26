#pragma once
#include "../../../OSDATA/userdata.h"
#include "../defaultInstance/defaultInstance.h"
#include "../../../customClasses/list.h"




class DebugTerminalInstance : public DefaultInstance
{
    private:
        Window* window;

    public:
    DebugTerminalInstance(Window* window);


};