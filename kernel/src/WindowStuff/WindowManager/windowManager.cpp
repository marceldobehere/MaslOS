#include "windowManager.h"

namespace WindowManager
{

    Window* getWindowAtMousePosition()
    {
        for (uint64_t i = 0; i < osData.windows.getCount(); i++)
        {
            Position tl = osData.windows[i]->position;
            Position br = Position(tl.x + osData.windows[i]->size.width, tl.y + osData.windows[i]->size.height);
            
            tl.x--;
            tl.y--;
            br.x++;
            br.y++;

            if (MousePosition.x >= tl.x && MousePosition.x <= br.x && MousePosition.y >= tl.y && MousePosition.y <=br.y)
                return osData.windows[i];
        }
        return NULL;   
    }



}

