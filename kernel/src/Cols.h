#pragma once

#include <stdint.h>

    static struct col
    { 
        static const uint32_t 
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
        bred =   0xffFF0000,
        bblue =  0xff0000FF,
        bgreen = 0xff00FF00;
    } Colors;
    
    static const int colCount = 19;

    static const char* colNames[colCount] =
    {
        "white",
        "silver",
        "gray",
        "black",
        "pink",
        "green",
        "red",
        "purple",
        "orange",
        "cyan",
        "yellow",
        "brown",
        "blue",
        "bred",
        "bblue",
        "bgreen",
        "bright red",
        "bright blue",
        "bright green"
    };

    static uint32_t colValues[colCount] = 
    {
        Colors.white,
        Colors.silver,
        Colors.gray,
        Colors.black,
        Colors.pink,
        Colors.green,
        Colors.red,
        Colors.purple,
        Colors.orange,
        Colors.cyan,
        Colors.yellow,
        Colors.brown,
        Colors.blue,
        Colors.bred,
        Colors.bblue,
        Colors.bgreen,
        Colors.bred,
        Colors.bblue,
        Colors.bgreen
    };