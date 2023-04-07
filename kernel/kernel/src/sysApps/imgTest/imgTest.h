#pragma once
#include "../../WindowStuff/Window/window.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/box/boxComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/textField/textFieldComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/button/buttonComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/imageRect/imageRectangleComponent.h"

namespace SysApps
{

    class ImageViewer
    {
        private:
        Window* window;
        GuiInstance* guiInstance;
        GuiComponentStuff::ImageRectangleComponent* testImg1;
        GuiComponentStuff::ButtonComponent* openBtn;

        public:
        ImageViewer(const char* path);

        void OnExternalWindowClose(Window* window);
        void OnExternalWindowResize(Window* window);

        void OnOpenClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnTaskDone(Task* task);


        void Free();
    };



}