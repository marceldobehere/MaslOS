#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"
#include "../../../../../../customClasses/list.h"

namespace GuiComponentStuff
{
    class ScreenComponent : public BaseComponent
    {
        private:
        Window* window;

        public:
        List<BaseComponent*>* children;
        uint32_t backgroundColor = 0;

        ScreenComponent(Window* window);
        void MouseClicked(Position mousePos);
        void Render(Field field);
        void Destroy(bool destroyChildren);
        ComponentSize GetActualComponentSize(BaseComponent* caller);
    };
}