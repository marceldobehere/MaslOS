#pragma once
#include "../../WindowStuff/Window/window.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"


namespace SysApps
{
    class Explorer
    {
        private:
        Window* window;
        GuiInstance* guiInstance;
        GuiComponentStuff::BaseComponent* lastClickedComp;
        
        const char* path;


        public:
        Explorer();
        const char* GetPath();
        void SetPath(const char* path);
        void Reload();

        void ClickCallBack(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::MouseClickEventInfo event);


        void OnExternalWindowClose(Window* window);
        void Free();
    };



}