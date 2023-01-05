#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"
#include "../../../../../../customClasses/list.h"

namespace GuiComponentStuff
{
    enum class GuiInstanceScreenComponentAttributeType : int32_t
    {
        WINDOW_ID = 10,
        BACKGROUND_COLOR = 11,
        SELECTED_COMPONENT_ID = 12
    };

    class ScreenComponent : public BaseComponent
    {
        private:
        BaseComponent* selectedComponent = NULL;

        public:
        Window* window;
        List<BaseComponent*>* children;
        uint32_t backgroundColor = 0;
        BaseComponent* tempSelectedComponent = NULL;

        ScreenComponent(Window* window);
        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);
        void Render(Field field);
        void Destroy(bool destroyChildren);
        ComponentSize GetActualComponentSize();
    };
}