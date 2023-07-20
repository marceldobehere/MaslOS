#pragma once
#include "../../WindowStuff/Window/window.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/canvas/canvasComponent.h"
#include "../../cStdLib/list/list_basics.h"

namespace SysApps
{

    class MemUsageShower
    {
        private:
        Window* window;
        GuiInstance* guiInstance;
        GuiComponentStuff::CanvasComponent* canvas;
        List<int64_t>* memUsageList;
        int64_t lastTimeMS;
        int wantedListCount;



        public:
        MemUsageShower();

        void OnExternalWindowClose(Window* window);
        void OnExternalWindowResize(Window* window);
        void OnExternalWindowFrame(Window* window);
        void DoRender();

    


        void Free();
    };



}