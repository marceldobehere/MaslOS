#include "windowManager.h"

namespace WindowManager
{

    Window* getWindowAtMousePosition()
    {
        //GlobalRenderer->Println("Mouse POS Check");
        for (int64_t i = osData.windows.getCount() - 1; i >= 0; i--)
        {
            //GlobalRenderer->Println("Check: {}", to_string(i), Colors.yellow);
            Position tl = osData.windows[i]->position;
            Position br = Position(tl.x + osData.windows[i]->size.width, tl.y + osData.windows[i]->size.height);
            
            tl.x--;
            tl.y -= 22;
            br.x++;
            br.y++;

            if (MousePosition.x >= tl.x && MousePosition.x <= br.x && MousePosition.y >= tl.y && MousePosition.y <=br.y)
                return osData.windows[i];
        }
        //GlobalRenderer->Println("NONE");
        return NULL;   
    }



}

