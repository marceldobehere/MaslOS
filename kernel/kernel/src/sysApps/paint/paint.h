#pragma once
#include "../../WindowStuff/Window/window.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/box/boxComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/textField/textFieldComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/button/buttonComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/imageRect/imageRectangleComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/canvas/canvasComponent.h"

namespace SysApps
{

    class Paint
    {
        private:
        Window* window;
        GuiInstance* guiInstance;
        GuiComponentStuff::BoxComponent* menuBox;
        GuiComponentStuff::ButtonComponent* openBtn;
        GuiComponentStuff::ButtonComponent* saveBtn;
        GuiComponentStuff::ButtonComponent* saveAsBtn;
        GuiComponentStuff::ButtonComponent* clearBtn;
        GuiComponentStuff::BoxComponent* colorBox;
        GuiComponentStuff::ButtonComponent* lastCol;
        // Colors will be added into the colorbox
        GuiComponentStuff::CanvasComponent* canvas;
        GuiComponentStuff::Position canvasPosition;

        MPoint oldMousePos;
        bool oldMouseLeftState;
        bool oldMouseRightState;
        bool currMouseLeftState;
        bool currMouseRightState;
        uint32_t currentColor;
        int curSize;

        public:
        Paint();

        void OnExternalWindowClose(Window* window);
        void OnExternalWindowResize(Window* window);

        void OnOpenClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnSaveClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnSaveAsClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnClearClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnColorClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnCanvasClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnExternalWindowFrame(Window* window);

        
        void OnTaskDone(Task* task);


        void Free();
    };



}