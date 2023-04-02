#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"
#include "../../../../../../customClasses/list_BaseComponent.h"
#include "../box/boxComponent.h"
#include "../text/textComponent.h"
#include "../rectangle/rectangleComponent.h"

namespace GuiComponentStuff
{
    enum class GuiInstanceTextFieldComponentAttributeType : int32_t
    {
        TEXT_COLOR = 10,
        BACKGROUND_COLOR = 11,

        MOUSE_HOVER = 21,
        MOUSE_CLICK = 22,

        BOX_COMPONENT_ID = 30,
        RECT_COMPONENT_ID = 31,
        TEXT_COMPONENT_ID = 32,
    };

    class TextFieldComponent : public BaseComponent
    {
        public:
        BoxComponent* actualTextFieldStuff;
        TextComponent* textComp;
        RectangleComponent* rectComp;
        
        uint32_t textCol;
        uint32_t bgCol;

        bool mouseHover = false;
        bool mouseClick = false;

        void (*mouseClickedCallBack)(BaseComponent*, MouseClickEventInfo);
        void (*keyHitCallBack)(BaseComponent*, KeyHitEventInfo);

        void* AdvancedKeyHitCallBackHelp;
        bool (*AdvancedKeyHitCallBack)(void* bruh, BaseComponent*, KeyHitEventInfo); // true if print

        TextFieldComponent(uint32_t textCol, uint32_t bgCol, ComponentSize size, Position position, BaseComponent* parent);
        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);
        void Render(Field field);
        void Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp));
        ComponentSize GetActualComponentSize();

        void KeyHandler(GuiComponentStuff::KeyHitEventInfo info);

        bool SetAttribute(int32_t type, uint64_t val);
        uint64_t GetAttribute(int32_t type);
        int GetAttributeSize(int32_t type);
    };
}