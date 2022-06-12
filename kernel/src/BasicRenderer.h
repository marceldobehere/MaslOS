#pragma once


#include "math.h"
#include "Framebuffer.h"
#include "S-Font.h"
#include <stdint.h> 
#include "dispTypes/dispTypes.h"
#include "ImageStruct.h"

class BasicRenderer
{
    public:
        
        Point CursorPosition;
        Framebuffer* framebuffer;
        PSF1_FONT* psf1_font;
        unsigned int color;
        bool overwrite = false;
        void putChar(char chr, int64_t xoff, int64_t yoff);
        void delChar(int64_t xoff, int64_t yoff, uint32_t col);
        void delChar(int64_t xoff, int64_t yoff);
        void putStr(const char* chrs, int64_t xoff, int64_t yoff);
        void putStr(const char *chrs, int64_t xoff, int64_t yoff, uint32_t col);

        void printStr(const char* chrs);
        void printStr(const char* chrs, bool allowEscape);
        void printStr(const char* chrs, const char* var);
        void printStr(const char* chrs, const char* var, bool allowEscape);
 
        void Println();

        void Print(char chr);
        void Print(const char* chrs);
        void Print(const char* chrs, bool allowEscape);
        void Println(const char* chrs);
        void Print(const char* chrs, const char* var);
        void Println(const char* chrs, const char* var);

        void Print(const char* chrs, dispVar vars[]); 
        void Println(const char* chrs, dispVar vars[]);
        void PrintArg(dispVar var);

        void Print(const char* chrs, uint32_t col);
        void Println(const char* chrs, uint32_t col);
        void Print(const char* chrs, const char* var, uint32_t col);
        void Println(const char* chrs, const char* var, uint32_t col);

        void Clear(uint32_t col);
        void Clear(int64_t x1, int64_t y1, int64_t x2, int64_t y2, uint32_t col);
        void Clear(uint32_t col, bool resetCursor);

        void DrawImage(ImageFile* image, int64_t x, int64_t y);

        void Cls();

        BasicRenderer(Framebuffer* framebuffer, PSF1_FONT* psf1_font);

        
};

extern BasicRenderer* GlobalRenderer;