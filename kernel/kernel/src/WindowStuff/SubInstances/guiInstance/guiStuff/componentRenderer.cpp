#include "componentRenderer.h"
#include "../../../../memory/heap.h"
#include "../../../../OSDATA/MStack/MStackM.h"


namespace GuiComponentStuff
{
    ComponentFramebuffer::ComponentFramebuffer(int x, int y)
    {
        this->pixels = (uint32_t*)_Malloc(x * y * sizeof(uint32_t));
        Width = x;
        Height = y;
    }

    ComponentFramebuffer::ComponentFramebuffer(int x, int y, uint32_t* pxls)
    {
        this->pixels = pxls;
        Width = x;
        Height = y;
    }

    void ComponentFramebuffer::Free()
    {
        _Free(pixels);
        _Free(this);
    }

    ComponentRenderer::ComponentRenderer(ComponentSize size)
    {
        componentFrameBuffer = new ComponentFramebuffer(size.FixedX, size.FixedY);
        Fill(0);
        font = GlobalRenderer->psf1_font;
    }

    void ComponentRenderer::Resize(ComponentSize size)
    {
        AddToStack();
        ComponentFramebuffer* temp = new ComponentFramebuffer(size.FixedX, size.FixedY);
        ComponentFramebuffer* old = componentFrameBuffer;

        componentFrameBuffer = temp;
        Fill(bgCol);
        componentFrameBuffer = old;
        
        Render(Position(0,0), Field(Position(0, 0), Position(old->Width - 1, old->Height - 1)), temp);

        componentFrameBuffer = temp;
        old->Free();
        RemoveFromStack();
    }

    void ComponentRenderer::Render(Position componentPosition, Field field, ComponentFramebuffer* to)
    {
        AddToStack();
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
            {
                uint32_t col = pxls[x + y * w];
                if (col >> 24 != 0)
                    pxlsTo[(x + componentPosition.x) + (y + componentPosition.y) * w2] = col;
            }
        RemoveFromStack();
        RemoveFromStack();
    }

    void ComponentRenderer::Fill(uint32_t col)
    {
        AddToStack();
        uint32_t* pxls = componentFrameBuffer->pixels;
        int h = componentFrameBuffer->Height;
        int w = componentFrameBuffer->Width;
        long t = h * (long)w;
        for (long x = 0; x < t; x++)
            pxls[x] = col;
        RemoveFromStack();
    }

    void ComponentRenderer::Fill(uint32_t col, Field field)
    {
        AddToStack();
        uint32_t* pxls = componentFrameBuffer->pixels;
        int h = componentFrameBuffer->Height;
        int w = componentFrameBuffer->Width;

        if (field.TL.x < 0)
            field.TL.x = 0;
        if (field.TL.y < 0)
            field.TL.y = 0;
        if (field.BR.x >= w)
            field.BR.x = w - 1;
        if (field.BR.y >= h)
            field.BR.y = h - 1;

        for (int y = field.TL.y; y <= field.BR.y; y++)
            for (int x = field.TL.x; x <= field.BR.x; x++)
                pxls[x + y * w] = col;
        RemoveFromStack();
    }

    void ComponentRenderer::Free()
    {
        componentFrameBuffer->Free();
        _Free(this);
    }

    Position ComponentRenderer::PrintString(char chr, Position pos, uint32_t fgCol, uint32_t bgCol, bool transparent)
    {
        AddToStack();
        if (chr != '\n')
        {
            uint32_t* pixPtr = (unsigned int *)componentFrameBuffer->pixels;
            char *fontPtr = ((char *)font->glyphBuffer) + (chr * font->psf1_Header->charsize);

            int eY = min(pos.y + 16, componentFrameBuffer->Height);
            int eX = min(pos.x + 8, componentFrameBuffer->Width);
            
            int pps = componentFrameBuffer->Width;

            for (int64_t y = max(pos.y, 0); y < eY; y++)
            {
                for (int64_t x = max(pos.x, 0); x < eX; x++)
                {
                    if ((*fontPtr & (0b10000000 >> (x - pos.x))) > 0)
                        *(unsigned int *)(pixPtr + x + (y * pps)) = fgCol;
                }
                fontPtr++;
            }
        }
        
        if (chr == '\n' || pos.x + 8 >= componentFrameBuffer->Width)
        {
            pos.x = 0;
            pos.y += 16;
        }
        else
        {   
            pos.x += 8;
        }

        RemoveFromStack();
        return pos;
    }

    Position ComponentRenderer::PrintString(const char *chrs, Position pos, uint32_t fgCol, uint32_t bgCol, bool transparent, bool center)
    {
        if (!center)
        {
            for (int i = 0; chrs[i] != 0; i++)
                pos = PrintString(chrs[i], pos, fgCol, bgCol, transparent);
            return pos;
        }

        int w = componentFrameBuffer->Width;

        char* bleh2 = StrCopy(chrs);

        char* thing = (char*)((uint64_t)bleh2);
        int count = 0;
        for (int i = 0; thing[i] != 0; i++)
        {
            if (thing[i] == '\n')
            {
                thing[i] = 0;
                count++;
            }
        }


        {
            int tCount = count;
            int oldI = 0;
            const char* bleh = bleh2;
            for (int i = 0; tCount >= 0; i++)
            {
                if (thing[i] == 0)
                {
                    if (tCount < 0)
                        break;
                    tCount--;
                    
                    int len = StrLen(bleh);
                    int tW = 8 * len;
                    

                    int tX = (w - tW) / 2;
                    pos.x = tX;
                    PrintString(bleh, pos, fgCol, bgCol, transparent, false);
                    bleh += len + 1;

                    if (tCount >= 0)
                        pos.y += 16;
                }

            }
            // for (int i = 0; chrs[i] != 0; i++)
            //     pos = PrintString(chrs[i], pos, fgCol, bgCol, transparent);
        }


        {
            int tCount = count;
            for (int i = 0; tCount > 0; i++)
            {
                if (thing[i] == 0)
                {
                    thing[i] = '\n';
                    tCount--;
                }
            }
        }

        _Free((void*)bleh2);

        return pos;
    }

    Position ComponentRenderer::PrintString(const char *chrs, Position pos, uint32_t fgCol)
    {
        return PrintString(chrs, pos, fgCol, Colors.black, false, false);
    }

    Position ComponentRenderer::PrintString(char chr, Position pos, uint32_t fgCol)
    {
        return PrintString(chr, pos, fgCol, Colors.black, false);
    }
}