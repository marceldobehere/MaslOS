#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"

namespace GuiComponentStuff
{
    enum class GuiInstanceTextComponentAttributeType : int32_t
    {
        TEXT_COLOR = 10,
        BACKGROUND_COLOR = 11,

        TEXT_ADDRESS = 20,

        CENTER_TEXT = 30,
        USE_FIXED_SIZE = 31,
    };

    class TextComponent : public BaseComponent
    {
        private:
        bool TextUpdate();

        public:
        uint32_t bgColor;
        uint32_t fgColor;
        uint32_t bgColorOld;
        uint32_t fgColorOld;
        const char* text;
        const char* oldText;
        bool center = false;
        bool oldCenter = false;
        bool useFixedSize = false;

        TextComponent(BaseComponent* parent, uint32_t bgColor, uint32_t fgColor, const char* text, Position position);
        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);
        void Render(Field field);
        void CheckUpdates();
        void Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp));
        ComponentSize GetActualComponentSize();

        bool SetAttribute(int32_t type, uint64_t val);
        uint64_t GetAttribute(int32_t type);
        int GetAttributeSize(int32_t type);
    };
}