#pragma once

#include <stdint.h>
#include "kbScancodeTranslation.h"
#include "../BasicRenderer.h"


void HandleKeyboard(uint8_t scancode)
{

    char ascii = QWERTYKeyboard::Translate(scancode, false);

    if (ascii != 0)
        GlobalRenderer->Print(ascii);


}


