#pragma once
#include <stdint.h>

namespace QWERTYKeyboard
{

    const char ASCIITable[] =
    {
         0 ,  0 , '1', '2',
        '3', '4', '5', '6',
        '7', '8', '9', '0', 
        '-', '=',  0 ,  0 ,
        'q', 'w', 'e', 'r',
        't', 'y', 'u', 'i',
        'o', 'p', '[', ']',
         0 ,  0 , 'a', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', ';',
        '\'','`',  0 , '\\',
        'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',',
        '.', '/',  0 , '*',
         0 , ' '
    };

    const char ShiftASCIITable[] =
    {
         0 ,  0 , '!', '"',
         35, '$', '%', '&',
        '/', '(', ')', '=', 
        '_', '=',  0 ,  0 ,
        'Q', 'W', 'E', 'R',
        'T', 'Y', 'U', 'I',
        'O', 'P', '{', '}',
         0 ,  0 , 'A', 'S',
        'D', 'F', 'G', 'H',
        'J', 'K', 'L', '\\',
        '\'','`',  0 , '\\',
        'Z', 'X', 'C', 'V',
        'B', 'N', 'M', ';',
        ':', '?',  0 , '~',
         0 , ' '
    };

    #define LeftShift   0x2A
    #define RightShift  0x36
    #define Enter       0x1C
    #define Backspace   0x0E
    #define ARR_UP      0x48 
    #define ARR_DOWN    0x50
    #define ARR_LEFT    0x4B
    #define ARR_RIGHT   0x4D
    

    char Translate(uint8_t scancode, bool uppercase)
    { 
        if (scancode > 58) return 0;

        if (uppercase) 
            return ShiftASCIITable[scancode];

        return ASCIITable[scancode];
    }


 
};

 