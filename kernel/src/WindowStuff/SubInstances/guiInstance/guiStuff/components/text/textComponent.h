#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"
#include "../../../../../../customClasses/list.h"

namespace GuiComponentStuff
{
    class TextComponent : public BaseComponent
    {
        private:
        bool TextUpdate();

        public:
        uint32_t bgColor;
        uint32_t fgColor;
        const char* text;
        const char* oldText;

        TextComponent(BaseComponent* parent, uint32_t bgColor, uint32_t fgColor, const char* text, Position position);
        void MouseClicked(Position mousePos);
        void Render(Field field);
        void Destroy(bool destroyChildren);
        ComponentSize GetActualComponentSize();
    };
}