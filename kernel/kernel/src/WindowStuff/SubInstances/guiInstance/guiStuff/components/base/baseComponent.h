#pragma once
#include <stdint.h>
#include "../../generalStuff.h"
#include "../../componentRenderer.h"
#include "../../../../../../customClasses/list.h"

namespace GuiComponentStuff
{
    enum ComponentType : uint8_t
    {
        NONE = 0,
        SCREEN = 1,
        RECT = 2,
        BOX = 3,
        TEXT = 4,
        BUTTON = 5,
        TEXTFIELD = 6
    };


    class BaseComponent
    {
        protected:
        List<Field>* updateFields;

        public:
        BaseComponent* parent;
        ComponentSize size = ComponentSize();
        Position position;
        ComponentSize oldSize = ComponentSize();
        Position oldPosition;
        //const char* ID;
        ComponentType componentType;
        ComponentRenderer* renderer;
        bool hidden = false;
        int64_t id = 10;
        




        public:

        BaseComponent(BaseComponent* parent);
        BaseComponent();
        ComponentSize GetActualComponentSize();
        Position GetAbsoluteComponentPosition();
        void* GetWindow();
        void* GetScreen();
        bool IsVisible();
        //BaseComponent* GetComponentFromId(uint64_t id);

        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);

        void Render(Field field);
        bool Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp));

        bool SetAttribute(int32_t type, uint64_t val);
        uint64_t GetAttribute(int32_t type);
        int GetAttributeSize(int32_t type);
        

    };


}