#include "kbScancodeTranslation.h"

namespace QWERTYKeyboard
{
    char Translate(uint8_t scancode, bool uppercase)
    { 
        if (scancode == ARR_UP) return 0xad;
        if (scancode == ARR_DOWN) return 0xaf;
        if (scancode == ARR_LEFT) return 0xac;
        if (scancode == ARR_RIGHT) return 0xae;


        if (scancode == Control) return 0x80 + 0x1d;
        if (scancode == LeftShift) return 0x80 + 0x36;
        if (scancode == RightShift) return 0x80 + 0x36;
        if (scancode == LeftAlt) return 0x80 + 0x38;
        if (scancode == Backspace) return 127;
        if (scancode == Enter) return 13;
        if (scancode == Escape) return 27;
        if (scancode == Tab) return 9;


        if (scancode >= 58) return 0;
        



        if (uppercase) 
            return ShiftASCIITable[scancode];

        return ASCIITable[scancode];
    }
}