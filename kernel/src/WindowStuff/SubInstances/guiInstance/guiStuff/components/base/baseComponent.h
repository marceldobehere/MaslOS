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
        RECT
    };


    class BaseComponent
    {
        protected:
        BaseComponent* parent;
        

        public:
        ComponentSize actualSize;
        ComponentSize wantedSize;
        Position position;
        const char* ID;
        ComponentType componentType;
        ComponentRenderer* renderer;



        public:

        BaseComponent(BaseComponent* parent);
        BaseComponent();
        ComponentSize GetActualComponentSize();

        void MouseClicked(Position mousePos);

        void Render(Field field);
        void Destroy(bool destroyChildren);

    };


}