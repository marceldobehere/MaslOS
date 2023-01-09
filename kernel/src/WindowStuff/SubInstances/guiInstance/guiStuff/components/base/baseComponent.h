#pragma once
#include <stdint.h>
#include "../../generalStuff.h"
#include "../../componentRenderer.h"

namespace GuiComponentStuff
{
    enum ComponentType
    {
        NONE,
        SCREEN,
        RECT,
        BOX,
        TEXT,
        BUTTON,
        TEXTFIELD
    };


    class BaseComponent
    {
        protected:
        

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
        BaseComponent* GetComponentFromId(uint64_t id);

        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);

        void Render(Field field);
        void Destroy(bool destroyChildren);

        bool SetAttribute(int32_t type, uint64_t val);
        uint64_t GetAttribute(int32_t type);

    };


}