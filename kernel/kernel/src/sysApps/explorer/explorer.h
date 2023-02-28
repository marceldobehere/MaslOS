#pragma once
#include "../../WindowStuff/Window/window.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/box/boxComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/textField/textFieldComponent.h"

namespace SysApps
{
    class Explorer
    {
        private:
        Window* window;
        GuiInstance* guiInstance;
        GuiComponentStuff::BaseComponent* lastClickedComp;
        GuiComponentStuff::BoxComponent* fileListComp;
        GuiComponentStuff::TextFieldComponent* pathComp;
        
        const char* path;


        public:
        Explorer();
        void Explorer::UpdateSizes();
        const char* GetPath();
        void SetPath(const char* path);
        void Reload();

        void ClickCallBack(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::MouseClickEventInfo event);
        bool PathTypeCallBack(GuiComponentStuff::TextFieldComponent* comp, GuiComponentStuff::KeyHitEventInfo event);

        void OnExternalWindowClose(Window* window);
        void Free();
    };



}