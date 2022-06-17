#include "windowManager.h"

namespace WindowManager
{

    Window* getWindowAtMousePosition(int dis)
    {
        AddToMStack(MStack("", "WindowStuff/WindowManager/windowManager.cpp"));
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

            tl.x -= dis;
            tl.y -= dis;
            br.x += dis;
            br.y += dis;


            if (MousePosition.x >= tl.x && MousePosition.x <= br.x && MousePosition.y >= tl.y && MousePosition.y <=br.y)
            {
                RemoveLastMStack();
                return osData.windows[i];
            }
        }
        //GlobalRenderer->Println("NONE");
        RemoveLastMStack();
        return NULL;   
    }

    Window* getWindowAtMousePosition()
    {
        return getWindowAtMousePosition(8);
    }

}

