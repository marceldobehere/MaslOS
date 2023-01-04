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
        BUTTON
    };


    class BaseComponent
    {
        protected:
        BaseComponent* parent;

        public:
        ComponentSize size = ComponentSize();
        Position position;
        ComponentSize oldSize = ComponentSize();
        Position oldPosition;
        const char* ID;
        ComponentType componentType;
        ComponentRenderer* renderer;
        bool hidden = false;
        bool newLine = false;



        public:

        BaseComponent(BaseComponent* parent);
        BaseComponent();
        ComponentSize GetActualComponentSize();
        Position GetAbsoluteComponentPosition();
        void* GetWindow();
        void* GetScreen();
        bool IsVisible();

        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);

        void Render(Field field);
        void Destroy(bool destroyChildren);

    };


}