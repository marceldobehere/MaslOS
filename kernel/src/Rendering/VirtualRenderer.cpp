#include "VirtualRenderer.h"
#include "../OSDATA/MStack/MStackM.h"


namespace VirtualRenderer
{
    PSF1_FONT* psf1_font;

    void DrawImage(kernelFiles::ImageFile* image, int64_t x, int64_t y, int64_t sx, int64_t sy, Border border, PointerFramebuffer* framebuffer)
    {
        x += image->xOff * sx;
        y += image->yOff * sy;

        uint64_t addr = (uint64_t)framebuffer->BaseAddress;
        uint64_t mult = framebuffer->Width;
        uint32_t* imgaddr = (uint32_t*)image->imageBuffer;
        for (int64_t y1 = 0; y1 < image->height; y1++)
        {
            for (int64_t x1 = 0; x1 < image->width; x1++)
            {
                if (*imgaddr != 0)//((*imgaddr/* | 0xffffff00*/) & (uint32_t)0xff000000 != (uint32_t)0x00000000)
                {
                    for (int iy = 0; iy < sy; iy++)
                    {
                        int64_t yp = (y1*sy) + iy + y;
                        for (int ix = 0; ix < sx; ix++)
                        {
                            int64_t xp = (x1*sx) + x + ix;
                            if (xp >= border.x1 && yp >= border.y1 && xp <= border.x2 && yp <= border.y2)
                                ((uint32_t**)addr)[xp + mult * yp] = imgaddr;
                        }
                    }
                }
                
                imgaddr ++;
            }
        }
    }

    void Clear(int64_t x1, int64_t y1, int64_t x2, int64_t y2, Border border, PointerFramebuffer* framebuffer, uint32_t* col)
    {
        uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
        uint64_t fbWidth = framebuffer->Width;
        uint64_t fbHeight = framebuffer->Height;

        // if (x1 < 0)
        //     x1 = 0;
        // if (y1 < 0)
        //     y1 = 0;
        // if (x2 >= framebuffer->Width)
        //     x2 = framebuffer->Width - 1;
        // if (y2 >= framebuffer->Height)
        //     y2 = framebuffer->Height - 1;  

        if (x1 < border.x1)
            x1 = border.x1;
        if (y1 < border.y1)
            y1 = border.y1;
        if (x2 > border.x2)
            x2 = border.x2;
        if (y2 > border.y2)
            y2 = border.y2;  


        for (int64_t x = x1; x <= x2; x++)
            for (int64_t y = y1; y <= y2; y++)
                ((uint32_t**)fbBase)[x + y * fbWidth] = (uint32_t*)col;
            
    }

    void putStr(const char *chrs, int64_t xoff, int64_t yoff, Border border, PointerFramebuffer* framebuffer, uint32_t* col)
    {
        for (unsigned int x = 0; chrs[x] != 0; x++)
            putChar(chrs[x], xoff + (x * 8), yoff, border, framebuffer, col);
    }

    void putChar(char chr, int64_t xoff, int64_t yoff, Border border, PointerFramebuffer* framebuffer, uint32_t* col)
    {
        uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
        uint64_t fbWidth = framebuffer->Width;
        uint64_t fbHeight = framebuffer->Height;
        char *fontPtr = ((char *)psf1_font->glyphBuffer) + (chr * psf1_font->psf1_Header->charsize);

        // if (overwrite)
        //     BasicRenderer::delChar(xoff, yoff);

        for (int64_t y = yoff; y < yoff + 16; y++)
        {
            for (int64_t x = xoff; x < xoff + 8; x++)
            {
                if (x >= border.x1 && x <= border.x2 && y >= border.y1 && y <= border.y2)
                    if ((*fontPtr & (0b10000000 >> (x - xoff))) > 0)
                        ((uint32_t**)fbBase)[x + y * fbWidth] = (uint32_t*)col;
            }
            fontPtr++;
        }
    }


    void DrawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2, Border border, PointerFramebuffer* framebuffer, uint32_t* col)
    {
        AddToStack("DrawLine", "VirtualRenderer.cpp");
        uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
        uint64_t fbWidth = framebuffer->Width;
        uint64_t fbHeight = framebuffer->Height;

        if (x1 > x2)
        {
            int64_t t = x1;
            x1 = x2;
            x2 = t;
        }

        if (y1 > y2)
        {
            //(y1, y2) = (y2, y1);
            int64_t t = y1;
            y1 = y2;
            y2 = t;
        }

        int64_t xdiff = (x2-x1) + 1;
        int64_t ydiff = (y2-y1) + 1;

        if (xdiff > ydiff)
        {
            for (int64_t x = x1; x <= x2; x++)
            {
                int64_t y = y1 + ((x-x1)*(ydiff))/(xdiff);

                if (x >= border.x1 && x <= border.x2 && y >= border.y1 && y <= border.y2)
                    ((uint32_t**)fbBase)[x + y * fbWidth] = (uint32_t*)col;
            }
        }
        else if (xdiff < ydiff)
        {
            //int64_t step = (ydiff+1)/(xdiff+1);
            //osData.debugTerminalWindow->Log("1");
            //osData.debugTerminalWindow->Log("XDIFF: {}", to_string(xdiff), Colors.yellow);
            //osData.debugTerminalWindow->Log("YDIFF: {}", to_string(ydiff), Colors.yellow);
            //osData.debugTerminalWindow->Log("STEP:  {}", to_string(step), Colors.yellow);


            for (int64_t y = y1; y <= y2; y++)
            {
                int64_t x = x1 + ((y-y1)*(xdiff))/(ydiff);

                if (x >= border.x1 && x <= border.x2 && y >= border.y1 && y <= border.y2)
                    ((uint32_t**)fbBase)[x + y * fbWidth] = (uint32_t*)col;
            }
        }
        else
        {
            int64_t y = y1;
            for (int64_t x = x1; x <= x2; x++)
            {
                if (x >= border.x1 && x <= border.x2 && y >= border.y1 && y <= border.y2)
                    ((uint32_t**)fbBase)[x + y * fbWidth] = (uint32_t*)col;
                y++;
            } 
        }
        RemoveFromStack();
    }
}