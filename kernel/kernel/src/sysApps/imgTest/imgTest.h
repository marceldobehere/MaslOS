#pragma once
#include "../../WindowStuff/Window/window.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/box/boxComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/textField/textFieldComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/button/buttonComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/imageRect/imageRectangleComponent.h"

namespace SysApps
{
    class ImageTest
    {
        private:
        Window* window;
        GuiInstance* guiInstance;
        GuiComponentStuff::ImageRectangleComponent* testImg1;

        public:
        ImageTest(const char* path);

        void OnExternalWindowClose(Window* window);

        void Free();
    };



}