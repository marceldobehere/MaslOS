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
        TEXT
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

        void MouseClicked(Position mousePos);

        void Render(Field field);
        void Destroy(bool destroyChildren);

    };


}