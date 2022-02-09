
#include "BasicRenderer.h"
#include "cstr.h"
#include <stdint.h> 


void BasicRenderer::putChar(char chr, unsigned int xoff, unsigned int yoff)
{
    unsigned int *pixPtr = (unsigned int*)framebuffer->BaseAddress;
    char* fontPtr = ((char*)psf1_font->glyphBuffer) + (chr * psf1_font->psf1_Header->charsize);

    for (unsigned long y = yoff; y < yoff + 16; y++)
    {
        for (unsigned long x = xoff; x < xoff + 8; x++)
        {
            if ((*fontPtr & (0b10000000 >> (x - xoff))) > 0)
            {
                *(unsigned int*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = color;

            }
        }
        fontPtr++;
    }

}

void BasicRenderer::putStr(const char* chrs, unsigned int xoff, unsigned int yoff)
{
    for (unsigned int x = 0; chrs[x] != 0; x++)
        putChar(chrs[x], xoff + (x * 8), yoff);
}

void BasicRenderer::printStr(const char* chrs)
{
    printStr(chrs, NULL);
}


void BasicRenderer::printStr(const char* chrs, const char* var)
{
    unsigned int index = 0;
    while (chrs[index] != 0)   
    {
        if (CursorPosition.x >= framebuffer->Width)
        {
            CursorPosition.x = 0;
            CursorPosition.y += 16;
        }


        if (chrs[index] == '\n')
        {
            CursorPosition.y += 16;
        }
        else if (chrs[index] == '\r')
        {
            CursorPosition.x = 0;
        }
        else if (chrs[index] == '{')
        {
            if (chrs[index + 1] == '}')
            {
                printStr(var);
                index++;
            }
        }
        else if (chrs[index] == '\\')
        {
            if (chrs[index + 1] == '\\')
            {
                index++;
                putChar(chrs[index], CursorPosition.x, CursorPosition.y);
                CursorPosition.x += 8;
            }
            else if (chrs[index + 1] == '%')
            {
                index++;
                putChar('%', CursorPosition.x, CursorPosition.y);
                CursorPosition.x += 8;
            }
            else if (chrs[index + 1] == '{')
            {
                index++;
                putChar('{', CursorPosition.x, CursorPosition.y);
                CursorPosition.x += 8;
            }
            else if (chrs[index + 1] == '}')
            {
                index++;
                putChar('{', CursorPosition.x, CursorPosition.y);
                CursorPosition.x += 8;
            }
            else if (chrs[index + 1] == 'C')
            {
                index++;
                if (chrs[index + 1] == 0 || chrs[index + 2] == 0 || chrs[index + 3] == 0 || chrs[index + 4] == 0 || chrs[index + 5] == 0 || chrs[index + 6] == 0)
                {
                    putChar('?', CursorPosition.x, CursorPosition.y);
                }
                else
                {
                    index++;
                    color = ConvertStringToHex(&chrs[index]);
                    index += 5;
                }
            }
        }
        else
        {
            putChar(chrs[index], CursorPosition.x, CursorPosition.y);
            CursorPosition.x += 8;
        }

        index++;
    }
}

void BasicRenderer::Print(const char* chrs)
{
    BasicRenderer::printStr(chrs);
}

void BasicRenderer::Println(const char* chrs)
{
    BasicRenderer::printStr(chrs);
    BasicRenderer::printStr("\n\r");
}

void BasicRenderer::Print(const char* chrs, const char* var)
{
    BasicRenderer::printStr(chrs, var);
}

void BasicRenderer::Println(const char* chrs, const char* var)
{
    BasicRenderer::printStr(chrs, var);
    
    BasicRenderer::printStr("\n\r");
}


void BasicRenderer::Println()
{
    BasicRenderer::printStr("\n\r");
}