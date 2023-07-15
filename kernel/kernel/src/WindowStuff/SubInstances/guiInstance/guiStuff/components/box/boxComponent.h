#pragma once
#include "../base/baseComponent.h"
#include "../../../../../Window/window.h"

namespace GuiComponentStuff
{

    enum class GuiInstanceBoxComponentAttributeType : int32_t
    {
        BACKGROUND_COLOR = 10
    };

    class BoxComponent : public BaseComponent
    {
        public:
        uint32_t backgroundColor = 0;
        List<BaseComponent*>* children;
        List<Field>* childrenFields;
        List<bool>* childrenHidden;

        //BoxComponent(BaseComponent* parent);
        BoxComponent(BaseComponent* parent, ComponentSize size, uint32_t bgCol);
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