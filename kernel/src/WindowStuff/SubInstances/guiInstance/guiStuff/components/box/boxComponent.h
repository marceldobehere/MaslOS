#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"
#include "../../../../../../customClasses/list.h"

namespace GuiComponentStuff
{
    class BoxComponent : public BaseComponent
    {
        public:
        uint32_t backgroundColor = 0;
        List<BaseComponent*>* children;

        //BoxComponent(BaseComponent* parent);
        BoxComponent(BaseComponent* parent, ComponentSize size, uint32_t bgCol);
        void MouseClicked(Position mousePos);
        void Render(Field field);
        void Destroy(bool destroyChildren);
        ComponentSize GetActualComponentSize();
    };
}