#include "cmdParser.h"
#include "../BasicRenderer.h"
#include "cstrTools.h"
#include "../Cols.h"
#include <stdlib.h>
#include "../paging/PageFrameAllocator.h"

void ParseCommand(char* input)
{
    GlobalRenderer->Println("This is test out!");
    if (StrEquals(input, "cls"))
        GlobalRenderer->Cls();

    //GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);
    StringArrData* data = SplitLine(input);
    //GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);


    GlobalRenderer->Println("Parts:");
    for (int i = 0; i < data->len; i++)
        GlobalRenderer->Println(" - \"{}\"", data->data[i]);


    //free(splitLine);
    GlobalAllocator->FreePage(data);
    //GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);

}

StringArrData* SplitLine(char* input)
{
    int index = 0;
    int parts[100];
    int partIndex = 0;

    for (int i = 0; i < 100; i++)
        parts[i] = 0;

    bool inString = false;

    for (; input[index] != 0; index++)
    {
        if (input[index] == '\"')
            inString = !inString;
        else if (!inString && input[index] == ' ')
            partIndex++;
        else 
        {
            parts[partIndex]++;
            if (input[index] == '\\')
                index++;
        }
    }

    int partCount = partIndex + 1;

    StringArrData* data = (StringArrData*) GlobalAllocator->RequestPage();

    //char** splitLine = (char**) GlobalAllocator->RequestPage(); //(char**)calloc(partCount, sizeof(char*));

    data->data = (char**)((char*)data+sizeof(StringArrData));

    char** splitLine = data->data;
    data->len = partCount;

    int splitIndex = 0;
    for (int i = 0; i < partCount; i++)
    {
        splitLine[i] = (char*)((char*)splitLine + splitIndex);
        splitIndex += parts[i];
    }
    
    // ---------------------------------------------------------------------------------------------------------------------------------

    inString = false;
    partIndex = 0;
    index = 0;

    for (int i = 0; i < 100; i++)
        parts[i] = 0;

    for (; input[index] != 0; index++)
    {
        if (input[index] == '\"')
            inString = !inString;
        else if (!inString && input[index] == ' ')
            partIndex++;
        else 
        {
            if (input[index] == '\\')
            {
                index++;
                splitLine[partIndex][parts[partIndex]] = input[index];
            }
            else
                splitLine[partIndex][parts[partIndex]] = input[index];
            
            parts[partIndex]++;
        }
    }
    


    return data;
}