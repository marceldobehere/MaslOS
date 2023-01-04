#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"
#include "../../../../../../customClasses/list.h"

namespace GuiComponentStuff
{
    class ScreenComponent : public BaseComponent
    {
        private:
        

        public:
        Window* window;
        List<BaseComponent*>* children;
        uint32_t backgroundColor = 0;
        BaseComponent* selectedComponent = NULL;

        ScreenComponent(Window* window);
        void MouseClicked(Position mousePos);
        void Render(Field field);
        void Destroy(bool destroyChildren);
        ComponentSize GetActualComponentSize();
    };
}