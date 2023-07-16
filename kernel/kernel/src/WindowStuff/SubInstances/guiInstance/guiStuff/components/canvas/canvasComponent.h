#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"
#include "../../../../../../cStdLib/list.h"

namespace GuiComponentStuff
{
    enum class GuiInstanceCanvasComponentAttributeType : int32_t
    {
        BG_COL = 10
    };

    class CanvasComponent : public BaseComponent
    {
        public:
        uint32_t bgColor;

        CanvasComponent(uint32_t bgColor, ComponentSize size, BaseComponent* parent);
        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);
        void Render(Field field);
        void CheckUpdates();
        void Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp));
        ComponentSize GetActualComponentSize();

        void* OnMouseClickHelp;
        void (*OnMouseClickedCallBack)(void*, BaseComponent*, MouseClickEventInfo);

        void Clear();
        void UpdateCanvas();
        void UpdateCanvas(int x, int y);
        void UpdateCanvas(int x, int y, int w, int h);
        void UpdateCanvas(Field field);
        

        bool SetAttribute(int32_t type, uint64_t val);
        uint64_t GetAttribute(int32_t type);
        int GetAttributeSize(int32_t type);
    };
}