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
        't', 'z', 'u', 'i',
        'o', 'p', '[', ']',
         0 ,  0 , 'a', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', ';',
        '\'','`',  0 , '\\',
        'y', 'x', 'c', 'v',
        'b', 'n', 'm', ',',
        '.', '/',  0 , '*',
         0 , ' '
    };
    char Translate(uint8_t scancode, bool uppercase)
    {
        if (scancode > 58) return 0;

        return ASCIITable[scancode];
    }



};

 