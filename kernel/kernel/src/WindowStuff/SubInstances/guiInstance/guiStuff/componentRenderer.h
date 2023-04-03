#pragma once
#include "generalStuff.h"
#include "../../../../Rendering/S-Font.h"

namespace GuiComponentStuff
{
    struct ComponentFramebuffer
    {
        uint32_t* pixels;
        unsigned int Width;
        unsigned int Height;

        ComponentFramebuffer(int x, int y);
        ComponentFramebuffer(int x, int y, uint32_t* pxls);
        void Free();
    };
    
    class ComponentRenderer
    {
        public:
        ComponentFramebuffer* componentFrameBuffer;
        uint32_t bgCol = 0;
        PSF1_FONT* font;


        ComponentRenderer(ComponentSize size);
        void Resize(ComponentSize size);
        void Render(Position componentPosition, Field field, ComponentFramebuffer* to);
        void Fill(uint32_t col);
        void Fill(uint32_t col, Field field);

        Position PrintString(char chr, Position pos, uint32_t fgCol, uint32_t bgCol, bool transparent);
        Position PrintString(const char *chrs, Position pos, uint32_t fgCol, uint32_t bgCol, bool transparent, bool center);
        Position PrintString(char chr, Position pos, uint32_t fgCol);
        Position PrintString(const char *chrs, Position pos, uint32_t fgCol);

        void Free();
        
    };
    
}