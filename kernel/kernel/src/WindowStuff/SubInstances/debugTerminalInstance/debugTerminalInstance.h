#pragma once
#include "../../../osData/userData.h"
#include "../defaultInstance/defaultInstance.h"
//#include "../../../cStdLib/list.h"
#include "../../Window/window.h"



class DebugTerminalInstance : public DefaultInstance
{
    private:
        Window* window;

    public:
    DebugTerminalInstance(Window* window);


};