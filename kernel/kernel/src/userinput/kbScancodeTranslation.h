#pragma once
#include <stdint.h>

namespace QWERTYKeyboard
{

    const char ASCIITable[] =
    {
        0 ,  0 , '1', '2',
        '3', '4', '5', '6',
        '7', '8', '9', '0', 
        '-', '+', '\b',  0 ,
        'q', 'w', 'e', 'r',
        't', 'y', 'u', 'i',
        'o', 'p', '[', ']',
        '\n',  0 , 'a', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', '<',
        '\'','`',  0 , '\\',
        'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',',
        '.', '>',  0 , '*',
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

    const uint8_t Scancode2ToScancode1[] = 
    {
        0x00, 0x43, 0x00, 0x3F, 0x3D, 0x3B, 0x3C, 0x58, 0x00, 0x44, 0x42, 0x40, 0x3E, 0x0F, 0x29, 0x00, 
        0x00, 0x38, 0x2A, 0x00, 0x1D, 0x10, 0x02, 0x00, 0x00, 0x00, 0x2C, 0x1F, 0x1E, 0x11, 0x03, 0x00, 
        0x00, 0x2E, 0x2D, 0x20, 0x12, 0x05, 0x04, 0x00, 0x00, 0x39, 0x2F, 0x21, 0x14, 0x13, 0x06, 0x00, 
        0x00, 0x31, 0x30, 0x23, 0x22, 0x15, 0x07, 0x00, 0x00, 0x00, 0x32, 0x24, 0x16, 0x08, 0x09, 0x00, 
        0x00, 0x33, 0x25, 0x17, 0x18, 0x0B, 0x0A, 0x00, 0x00, 0x34, 0x35, 0x26, 0x27, 0x19, 0x0C, 0x00, 
        0x00, 0x00, 0x28, 0x00, 0x1A, 0x0D, 0x00, 0x00, 0x3A, 0x36, 0x1C, 0x1B, 0x00, 0x2B, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x4F, 0x00, 0x4B, 0x47, 0x00, 0x00, 0x00, 
        0x52, 0x53, 0x50, 0x4C, 0x4D, 0x48, 0x01, 0x45, 0x57, 0x4E, 0x51, 0x4A, 0x37, 0x49, 0x46, 0x00, 
        0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    #define LeftShift   0x2A
    #define RightShift  0x36
    #define Enter       0x1C
    #define Backspace   0x0E
    #define ARR_UP      0x48 
    #define ARR_DOWN    0x50
    #define ARR_LEFT    0x4B
    #define ARR_RIGHT   0x4D
    #define LeftAlt   0x38
    

    char Translate(uint8_t scancode, bool uppercase)
    { 
        if (scancode > 58) return 0;



        if (uppercase) 
            return ShiftASCIITable[scancode];

        return ASCIITable[scancode];
    }


 
};

 