#include "cmdParser.h"
#include "../BasicRenderer.h"
#include "cstrTools.h"
#include "../Cols.h"
#include <stdlib.h>
#include <stdint.h>
#include "../paging/PageFrameAllocator.h"
#include "../userinput/keyboard.h"

void LogError(const char* msg)
{
    GlobalRenderer->Println(msg, Colors.bred);
}

void LogError(const char* msg, const char* var)
{
    GlobalRenderer->Println(msg, var, Colors.bred);
}

void LogInvalidArgumentCount(int expected, int found)
{
    GlobalRenderer->Print("Invalid Argument count. Expected {} but got ", to_string((uint64_t)expected), Colors.bred);
    GlobalRenderer->Println("{} instead.", to_string((uint64_t)found), Colors.bred);
}




void ParseCommand(char* input)
{
    //GlobalRenderer->Println("This is test out!");
    if (StrEquals(input, "cls"))
    {
        GlobalRenderer->Cls();
        return;
    }


    StringArrData* data = SplitLine(input);

    if (data->len == 0)
    {
        GlobalAllocator->FreePage(data);
        return;
    }

    // GlobalRenderer->Println("Parts:");
    // for (int i = 0; i < data->len; i++)
    //     GlobalRenderer->Println(" - \"{}\"", data->data[i], Colors.bgreen);

    if (StrEquals(data->data[0], "echo"))
    {
        if (data->len == 2)
            GlobalRenderer->Println(data->data[1]);
        else
            LogInvalidArgumentCount(1, data->len-1);
        
        GlobalAllocator->FreePage(data);
        return;
    }

    if (StrEquals(data->data[0], "set"))
    {
        if (data->len == 3)
            SetCmd(data->data[1], data->data[2]);
        else
            LogInvalidArgumentCount(2, data->len-1);
        
        GlobalAllocator->FreePage(data);
        return;
    }


    LogError("Unknown command \"{}\"!", data->data[0]);
    GlobalAllocator->FreePage(data);
    return;
}


void SetCmd(const char* name, const char* val)
{
    if (StrEquals(name, "user color"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            userCol = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val);
    }
    else if (StrEquals(name, "default color"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            GlobalRenderer->color = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val);
    }
    else if (StrEquals(name, "username"))
    {
        userName = StrCopy(val);
    }
    else
    {
        LogError("Parameter \"{}\" does not exist.", name);
    }
}


ParsedColData ParseColor(const char* col)
{
    ParsedColData data = ParsedColData();
    data.parseSuccess = false;
    data.col = 0;

    for (int i = 0; i < colCount; i++)
        if (StrEquals(col, colNames[i]))
        {
            data.col = colValues[i];
            //GlobalRenderer->Println("Color 1: {}", ConvertHexToString(colValues[i]), Colors.white);
            data.parseSuccess = true;
            break;
        }
    
    //GlobalRenderer->Println("Color 2: {}", ConvertHexToString(data.col), Colors.white);

    return data;
}

/*
    //GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);
    StringArrData* data = SplitLine(input);
    //GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);


    GlobalRenderer->Println("Parts:");
    for (int i = 0; i < data->len; i++)
        GlobalRenderer->Println(" - \"{}\"", data->data[i], Colors.bgreen);


    //free(splitLine);
    GlobalAllocator->FreePage(data);
    //GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);
*/

StringArrData* SplitLine(char* input)
{
    uint64_t index = 0;
    uint64_t parts[100];
    uint64_t partIndex = 0;

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
                if (input[index + 1] != 0)
                    index++;
        }
    }

    int partCount = partIndex + 1;

    uint64_t datAddr = (uint64_t) GlobalAllocator->RequestPage();
    StringArrData* data = (StringArrData*) datAddr;

    //char** splitLine = (char**) GlobalAllocator->RequestPage(); //(char**)calloc(partCount, sizeof(char*));

    //GlobalRenderer->Println("ADDR OG:      {}",ConvertHexToString((uint64_t)data),Colors.cyan);

    //GlobalRenderer->Println("ADDR datAddr: {}",ConvertHexToString(datAddr),Colors.bgreen);

    data->addrOfData = (void*)(datAddr + sizeof(StringArrData));

    data->data = (char**)data->addrOfData;


    //GlobalRenderer->Println("ADDR of Data 1: {}",ConvertHexToString((uint64_t)data->data),Colors.cyan);
    //GlobalRenderer->Println("ADDR of Data 2: {}",ConvertHexToString((uint64_t)data->addrOfData),Colors.cyan);

    char** splitLine = data->data;
    data->len = partCount;

    // GlobalRenderer->Println("ADDR 1: {}", ConvertHexToString((uint64_t)splitLine), Colors.yellow);

    // GlobalRenderer->Println("ADDR 2: {}", ConvertHexToString((uint64_t)splitLine), Colors.yellow);

    uint64_t splitIndex = sizeof(char**) + sizeof(char*) * partCount;

    //GlobalRenderer->Println("ADDR of Data 3: {}",ConvertHexToString((uint64_t)data->addrOfData),Colors.cyan);

    for (int i = 0; i < partCount; i++)
    {
        //GlobalRenderer->Println("INDEX  {}", to_string((uint64_t)splitIndex), Colors.cyan); 
        //GlobalRenderer->Println("Count: {}", to_string((uint64_t)parts[i]), Colors.cyan);
        splitLine[i] = (char*)(data->addrOfData + splitIndex);
        //GlobalRenderer->Println("ADDR 1: {}", ConvertHexToString((uint64_t)&splitLine[i][0]), Colors.yellow);

        for (int i2 = 0; i2 < parts[i] + 1; i2++)
            splitLine[i][i2] = 0;
       
        //GlobalRenderer->Println("ADDR 2: {}", ConvertHexToString((uint64_t)splitLine[i]), Colors.yellow);
        //GlobalRenderer->Println("ADDR 3: {}", ConvertHexToString((uint64_t)data->addrOfData + splitIndex), Colors.yellow);
        splitIndex += parts[i] + 1;
    }

    // ---------------------------------------------------------------------------------------------------------------------------------

    inString = false;
    partIndex = 0;
    index = 0;

    for (int i = 0; i < 100; i++)
        parts[i] = 0;
    
    //GlobalRenderer->Println("AAA");
    //GlobalRenderer->Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
    //GlobalRenderer->Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);

    //GlobalRenderer->Println("Lines:", Colors.yellow);

    for (; input[index] != 0; index++)
    {
        if (input[index] == '\"')
            inString = !inString;
        else if (!inString && input[index] == ' ')
        {
            //GlobalRenderer->Println();
            //GlobalRenderer->Println("PartIndex:   {}", to_string(partIndex), Colors.cyan);
            //GlobalRenderer->Println("PartIndex 2: {}", to_string(parts[partIndex]), Colors.cyan);
            //GlobalRenderer->Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
            //GlobalRenderer->Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);
            splitLine[partIndex][parts[partIndex]] = 0;
            //GlobalRenderer->Println(" - \"{}\"", (const char*)splitLine[partIndex], Colors.yellow);
            //GlobalRenderer->Println(", Count: {}", to_string((uint64_t)parts[partIndex] + 1), Colors.yellow);
            partIndex++;
        }
        else 
        {
            if (input[index] == '\\')
                if (input[index + 1] != 0)
                    index++;
            
            //splitLine[partIndex][parts[partIndex]] = input[index];
            //*(splitLine[partIndex] + parts[partIndex]) = input[index];
            //GlobalRenderer->Print(input[index]);
            //GlobalRenderer->Println("Index:           {}", to_string(index), Colors.bgreen);
            //GlobalRenderer->Println("Part Index:      {}", to_string(partIndex), Colors.bgreen);
            //GlobalRenderer->Println("Part Part Index: {}", to_string(parts[partIndex]), Colors.bgreen);

            splitLine[partIndex][parts[partIndex]] = input[index];

            parts[partIndex]++;
        }
    }

    ///GlobalRenderer->Println(" - \"{}\"", (const char*)splitLine[partIndex], Colors.yellow);
    // GlobalRenderer->Println();
    // GlobalRenderer->Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
    // GlobalRenderer->Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);
    // GlobalRenderer->Print(" - \"{}\"", splitLine[partIndex], Colors.yellow);
    // GlobalRenderer->Println(", Count: {}", to_string((uint64_t)parts[partIndex] + 1), Colors.yellow);


    return data;
}