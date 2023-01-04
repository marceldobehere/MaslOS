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
        font = GlobalRenderer->psf1_font;
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
            {
                uint32_t col = pxls[x + y * w];
                if (col >> 24 != 0)
                    pxlsTo[(x + componentPosition.x) + (y + componentPosition.y) * w2] = col;
            }
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

    Position ComponentRenderer::PrintString(char chr, Position pos, uint32_t fgCol, uint32_t bgCol, bool transparent)
    {
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

        char* thing = (char*)((uint64_t)chrs);
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
            const char* bleh = chrs;
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