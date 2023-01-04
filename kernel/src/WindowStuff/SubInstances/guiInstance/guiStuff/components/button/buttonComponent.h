#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"
#include "../../../../../../customClasses/list.h"
#include "../box/boxComponent.h"
#include "../text/textComponent.h"
#include "../rectangle/rectangleComponent.h"

namespace GuiComponentStuff
{
    class ButtonComponent : public BaseComponent
    {
        public:
        BoxComponent* actualButtonStuff;
        TextComponent* textComp;
        RectangleComponent* rectComp;
        
        uint32_t textColDef;
        uint32_t textColHover;
        uint32_t textColClick;
        
        uint32_t bgColDef;
        uint32_t bgColHover; 
        uint32_t bgColClick;

        bool mouseHover = false;
        bool mouseClick = false;

        void (*callBack)(BaseComponent*);

        ButtonComponent(const char* text, uint32_t textColDef, uint32_t textColHover, uint32_t textColClick, uint32_t bgColDef, uint32_t bgColHover, uint32_t bgColClick, ComponentSize size, Position position, BaseComponent* parent);
        void MouseClicked(Position mousePos);
        void Render(Field field);
        void Destroy(bool destroyChildren);
        ComponentSize GetActualComponentSize();
    };
}