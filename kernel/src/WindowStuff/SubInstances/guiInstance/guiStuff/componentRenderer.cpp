#include "componentRenderer.h"
#include "../../../../memory/heap.h"
#include "../../../../OSDATA/MStack/MStackM.h"


namespace GuiComponentStuff
{
    ComponentFramebuffer::ComponentFramebuffer(int x, int y)
    {
        this->pixels = (uint32_t*)malloc(x * y * sizeof(uint32_t));
        Width = x;
        Height = y;
    }

    void ComponentFramebuffer::Free()
    {
        free(pixels);
        free(this);
    }

    ComponentRenderer::ComponentRenderer(ComponentSize size)
    {
        componentFrameBuffer = new ComponentFramebuffer(size.FixedX, size.FixedY);
        Fill(0);
    }

    void ComponentRenderer::Resize(ComponentSize size)
    {
        ComponentFramebuffer* temp = new ComponentFramebuffer(size.FixedX, size.FixedY);
        ComponentFramebuffer* old = componentFrameBuffer;

        componentFrameBuffer = temp;
        Fill(bgCol);
        componentFrameBuffer = old;
        
        Render(Position(0,0), Field(Position(0, 0), Position(old->Width - 1, old->Height - 1)), temp);

        componentFrameBuffer = temp;
        old->Free();
    }

    void ComponentRenderer::Render(Position componentPosition, Field field, ComponentFramebuffer* to)
    {
        if (to == NULL)
            return;
        
        uint32_t* pxls = componentFrameBuffer->pixels;
        uint32_t* pxlsTo = to->pixels;
        int h = componentFrameBuffer->Height;
        int w = componentFrameBuffer->Width;
        int h2 = to->Height;
        int w2 = to->Width;

        int x1 = field.TL.x;
        int y1 = field.TL.y;
        if (x1 < 0)
            x1 = 0;
        if (y1 < 0)
            y1 = 0;

        int x2 = field.BR.x;
        int y2 = field.BR.y;
        if (x2 >= w)
            x2 =  w - 1;
        if (y2 >= h)
            y2 = h - 1;

        if (x2 + componentPosition.x >= w2)
            x2 = w2 - 1 - componentPosition.x;
        if (x1 + componentPosition.x < 0)
            x1 = -componentPosition.x;

        if (y2 + componentPosition.y >= h2)
            y2 = h2 - 1 - componentPosition.y;
        if (y1 + componentPosition.y < 0)
            y1 = -componentPosition.y;
        
        AddToStack();
        for (int y = y1; y <= y2; y++)
            for (int x = x1; x <= x2; x++)
                pxlsTo[(x + componentPosition.x) + (y + componentPosition.y) * w2] = pxls[x + y * w];
        RemoveFromStack();
    }

    void ComponentRenderer::Fill(uint32_t col)
    {
        uint32_t* pxls = componentFrameBuffer->pixels;
        int h = componentFrameBuffer->Height;
        int w = componentFrameBuffer->Width;
        long t = h * (long)w;
        for (long x = 0; x < t; x++)
            pxls[x] = col;
    }

    void ComponentRenderer::Fill(uint32_t col, Field field)
    {
        uint32_t* pxls = componentFrameBuffer->pixels;
        int h = componentFrameBuffer->Height;
        int w = componentFrameBuffer->Width;

        for (int y = field.TL.y; y <= field.BR.y && y < h; y++)
            for (int x = field.TL.x; x <= field.BR.x && x < w; x++)
                pxls[x + y * w] = col;
    }

    void ComponentRenderer::Free()
    {
        componentFrameBuffer->Free();
    }
}