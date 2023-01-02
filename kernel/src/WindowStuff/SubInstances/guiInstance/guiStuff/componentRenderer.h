#pragma once
#include "generalStuff.h"

namespace GuiComponentStuff
{
    struct ComponentFramebuffer
    {
        uint32_t* pixels;
        unsigned int Width;
        unsigned int Height;

        ComponentFramebuffer(int x, int y);
        void Free();
    };
    
    class ComponentRenderer
    {
        public:
        ComponentFramebuffer* componentFrameBuffer;
        uint32_t bgCol = 0;


        ComponentRenderer(ComponentSize size);
        void Resize(ComponentSize size);
        void Render(Position componentPosition, Field field, ComponentFramebuffer* to);
        void Fill(uint32_t col);
        void Fill(uint32_t col, Field field);

        void Free();
        
    };
    
}