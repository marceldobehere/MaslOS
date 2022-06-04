#include "cmdParser.h"
#include "../BasicRenderer.h"
#include "cstrTools.h"
#include "../Cols.h"
#include <stdlib.h>

void ParseCommand(char* input)
{
    GlobalRenderer->Println("This is test out!");
    if (StrEquals(input, "cls"))
        GlobalRenderer->Cls();

    char** splitLine = SplitLine(input);


    free(splitLine);
}

char** SplitLine(char* input)
{
    int index = 0;
    int parts[100];
    int partIndex = 0;

    for (int i = 0; i < 100; i++)
        parts[i] = 0;

    bool inString = false;

    while (input[index] != 0)
    {
        if (input[index] == '\"')
            inString = !inString;
        else if (input[index] == '\\')
        {
            parts[partIndex]++;
            index++;
        }
        else if (input[index] == ' ' && !inString)
            partIndex++;
        else
            parts[partIndex]++;

        index++;
    }

    int partCount = partIndex + 1;

    char** splitLine = (char**)calloc(partCount, sizeof(char*));
    for (int i = 0; i < partCount; i++)
        splitLine[i] = (char*)calloc(parts[partCount], sizeof(char));
    

    // -----------------------------------------------------------------------

    index = 0;
    

    inString = false;

    while (input[index] != 0)
    {
        if (input[index] == '\"')
            inString = !inString;
        else if (input[index] == '\\')
        {
            parts[partIndex]++;
            index++;
        }
        else if (input[index] == ' ' && !inString)
            partIndex++;
        else
            parts[partIndex]++;

        index++;
    }

}