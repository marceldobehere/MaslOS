#include "cmdParser.h"
#include "../BasicRenderer.h"
#include "cstrTools.h"
#include "../Cols.h"

void ParseCommand(char* input)
{
    GlobalRenderer->Println("This is test out!");
    if (StrEquals(input, "cls"))
        GlobalRenderer->Cls();
}