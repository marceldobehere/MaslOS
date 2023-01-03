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
        BOX
    };


    class BaseComponent
    {
        protected:
        BaseComponent* parent;

        public:
        ComponentSize size = ComponentSize(true);
        Position position;
        ComponentSize oldSize = ComponentSize(true);
        Position oldPosition;
        const char* ID;
        ComponentType componentType;
        ComponentRenderer* renderer;
        bool hidden = false;
        bool newLine = false;



        public:

        BaseComponent(BaseComponent* parent);
        BaseComponent();
        ComponentSize GetActualComponentSize(BaseComponent* caller);

        void MouseClicked(Position mousePos);

        void Render(Field field);
        void Destroy(bool destroyChildren);

    };


}