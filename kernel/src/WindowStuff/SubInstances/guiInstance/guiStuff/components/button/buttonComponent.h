#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"
#include "../../../../../../customClasses/list.h"
#include "../box/boxComponent.h"
#include "../text/textComponent.h"
#include "../rectangle/rectangleComponent.h"

namespace GuiComponentStuff
{
    enum class GuiInstanceButtonComponentAttributeType : int32_t
    {
        TEXT_COLOR_DEFAULT = 10,
        TEXT_COLOR_HOVER = 11,
        TEXT_COLOR_CLICK = 12,

        BG_COLOR_DEFAULT = 20,
        BG_COLOR_HOVER = 21,
        BG_COLOR_CLICK = 22,

        STICK_TO_DEFAULT_COLS = 30,
        MOUSE_HOVER = 31,
        MOUSE_CLICK = 32,

        BOX_COMPONENT_ID = 40,
        RECT_COMPONENT_ID = 41,
        TEXT_COMPONENT_ID = 42,

    };

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

        bool stickToDefaultColor = false;
        bool mouseHover = false;
        bool mouseClick = false;

        void (*mouseClickedCallBack)(BaseComponent*, MouseClickEventInfo);
        void (*keyHitCallBack)(BaseComponent*, KeyHitEventInfo);

        ButtonComponent(const char* text, uint32_t textColDef, uint32_t textColHover, uint32_t textColClick, uint32_t bgColDef, uint32_t bgColHover, uint32_t bgColClick, ComponentSize size, Position position, BaseComponent* parent);
        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);
        void Render(Field field);
        void Destroy(bool destroyChildren);
        ComponentSize GetActualComponentSize();
    };
}