#pragma once
#include "../../../OSDATA/userdata.h"
#include "../defaultInstance/defaultInstance.h"
//#include "../../../customClasses/list.h"
#include "../../Window/window.h"
#include "guiStuff/components/screenComponent/screenComponent.h"


class GuiInstance : public DefaultInstance
{
    private:
        Window* window;
        

    public:
    GuiComponentStuff::ScreenComponent* screen;
    
    GuiInstance(Window* window);
    void Free();
    void Init();
    void Render();

};