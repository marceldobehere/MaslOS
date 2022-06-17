#pragma once

#include "../../userinput/mouse.h"
#include "../Window/window.h"  
#include "../../OSDATA/osdata.h" 

namespace WindowManager
{
    Window* getWindowAtMousePosition();
    Window* getWindowAtMousePosition(int distance);
}