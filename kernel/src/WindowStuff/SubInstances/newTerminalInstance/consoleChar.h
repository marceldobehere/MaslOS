#pragma once
#include <stdint.h>

struct ConsoleChar 
{
    char chr;
    uint32_t fg, bg;

    ConsoleChar(char chr, uint32_t fg, uint32_t bg)
    {
        this->chr = chr;
        this->fg = fg;
        this->bg = bg;
    }

    bool operator==(const ConsoleChar & other)
    {
        return  this->chr == other.chr &&
                this->fg == other.fg   &&
                this->bg == other.bg;
    }
};