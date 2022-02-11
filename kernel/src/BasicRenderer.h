#pragma once


#include "math.h"
#include "Framebuffer.h"
#include "S-Font.h"
#include <stdint.h> 


class BasicRenderer
{
    public:
        
        Point CursorPosition;
        Framebuffer* framebuffer;
        PSF1_FONT* psf1_font;
        unsigned int color;
        void putChar(char chr, unsigned int xoff, unsigned int yoff);
        void putStr(const char* chrs, unsigned int xoff, unsigned int yoff);

        void printStr(const char* chrs);
        void printStr(const char* chrs, const char* var);

        void Println();

        void Print(const char* chrs);
        void Println(const char* chrs);
        void Print(const char* chrs, const char* var);
        void Println(const char* chrs, const char* var);


        enum Colors
        { 
            white =  0xffffffff,
            silver = 0xffc0c0c0,
            gray =   0xff808080,
            black =  0xff000000,
            pink =   0xffFF1493,
            green =  0xff008000,
            red =    0xff800000,
            purple = 0xff800080,
            orange = 0xffFF4500,
            cyan =   0xff008080,
            yellow = 0xffFFD700,
            brown =  0xffA52A2A,
            blue =   0xff000080,
        };

        void Print(const char* chrs, Colors col);
        void Println(const char* chrs, Colors col);
        void Print(const char* chrs, const char* var, Colors col);
        void Println(const char* chrs, const char* var, Colors col);




        BasicRenderer(Framebuffer* framebuffer, PSF1_FONT* psf1_font)
        {
            color = 0xffffffff;
            CursorPosition = {0, 0};
            this->framebuffer = framebuffer;
            this->psf1_font = psf1_font;
        }

};