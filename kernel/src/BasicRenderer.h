#pragma once


#include "math.h"
#include "Framebuffer.h"
#include "S-Font.h"
#include <stdint.h> 
#include "dispTypes/dispTypes.h"

class BasicRenderer
{
    public:
        
        Point CursorPosition;
        Framebuffer* framebuffer;
        PSF1_FONT* psf1_font;
        unsigned int color;
        bool overwrite = false;
        void putChar(char chr, unsigned int xoff, unsigned int yoff);
        void delChar(unsigned int xoff, unsigned int yoff, uint32_t col);
        void delChar(unsigned int xoff, unsigned int yoff);
        void putStr(const char* chrs, unsigned int xoff, unsigned int yoff);

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
        void Clear(uint32_t col, bool resetCursor);

        void Cls();


        BasicRenderer(Framebuffer* framebuffer, PSF1_FONT* psf1_font);

        
};

extern BasicRenderer* GlobalRenderer;