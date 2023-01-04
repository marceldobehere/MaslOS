#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"
#include "../../../../../../customClasses/list.h"

namespace GuiComponentStuff
{
    class RectangleComponent : public BaseComponent
    {
        public:
        uint32_t fillColor;
        uint32_t oldFillColor;

        RectangleComponent(uint32_t fillColor, ComponentSize size, BaseComponent* parent);
        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);
        void Render(Field field);
        void Destroy(bool destroyChildren);
        ComponentSize GetActualComponentSize();
    };
}