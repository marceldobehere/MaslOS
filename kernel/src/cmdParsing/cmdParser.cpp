#include "cmdParser.h"
#include "../BasicRenderer.h"
#include "cstrTools.h"
#include "../Cols.h"
#include <stdint.h>
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"
#include "../userinput/keyboard.h"
#include "../userinput/mouse.h"
#include "../OSDATA/osdata.h"
#include "../scheduling-pit/pit.h"

void LogError(const char* msg, Window* window)
{
    window->renderer->Println(msg, Colors.bred);
}

void LogError(const char* msg, const char* var, Window* window)
{
    window->renderer->Println(msg, var, Colors.bred);
}

void LogInvalidArgumentCount(int expected, int found, Window* window)
{
    window->renderer->Print("Invalid Argument count. Expected {} but got ", to_string((uint64_t)expected), Colors.bred);
    window->renderer->Println("{} instead.", to_string((uint64_t)found), Colors.bred);
}




void ParseCommand(char* input, char* oldInput, OSUser** user, Window* window)
{
    //window->renderer->Println("This is test out!");
    if (StrEquals(input, "cls"))
    {
        window->renderer->Cls();
        return;
    }

    if (StrEquals(input, "malloc"))
    {
        malloc(0x5000);
        return;
    }

    if (StrEquals(input, "exit"))
    {
        window->renderer->Println("Exiting...");
        osData.exit = true;
        return;
    }

    //return;

    StringArrData* data = SplitLine(oldInput);

    if (StrEquals(data->data[0], "login") && (*user)->mode == commandMode::enterPassword)
    {
        (*user)->mode = commandMode::none;
        StringArrData* data2 = SplitLine(input);
        if (data->len == 2)
        {
            if (data2->len == 1)
                login(data->data[1], data2->data[0], user, window);
            else
                LogError("Password can only be one Argument long!", window);
        }
        
        free(data);
        free(data2);
        return;
    }

    if (StrEquals(data->data[0], "set") && (*user)->mode == commandMode::enterPassword)
    {
        (*user)->mode = commandMode::none;
        StringArrData* data2 = SplitLine(input);
        if (data->len == 2 || data->len == 3)
        {
            if (StrEquals(data->data[1], "password"))
            {
                if (data2->len == 1)
                {
                    (*user)->password = StrCopy(data2->data[0]);
                    //window->renderer->Println("Password is now \"{}\".", (*user)->password, Colors.yellow);
                }
                else
                    LogError("Password can only be one Argument long!", window);
            }
        }
        
        free(data);
        free(data2);
        return;
    }

    free(data);
    data = SplitLine(input);

    if (data->len == 0)
    {
        free(data);
        return;
    }

    // window->renderer->Println("Parts:");
    // for (int i = 0; i < data->len; i++)
    //     window->renderer->Println(" - \"{}\"", data->data[i], Colors.bgreen);

    if (StrEquals(data->data[0], "echo"))
    {
        if (data->len == 2)
            window->renderer->Println(data->data[1]);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        free(data);
        return;
    }

    if (StrEquals(data->data[0], "sleep"))
    {
        if (data->len == 2)
        {
            int64_t time = to_int(data->data[1]);
            if (time > 0)
            {
                window->renderer->Println("Sleeping for {} ms...", to_string(time), Colors.yellow);
                //PIT::Sleep((uint64_t)time);
                {

                    // window->renderer->Println("TIME: {} s", to_string(PIT::TimeSinceBoot), Colors.bred);
                    // window->renderer->Println("TIME: {} ms", to_string((int)(PIT::TimeSinceBoot*1000)), Colors.bred);
                    // window->renderer->Println("DIV:  {}", to_string(PIT::Divisor), Colors.bred);
                    // window->renderer->Println("FREQ: {} Hz", to_string(PIT::freq), Colors.bred);

                    // window->renderer->Println();

                    // for (int i = 0; i < 20; i++)
                    // {
                    //     window->renderer->Print("hoi! ");
                    //     PIT::Sleep(100);
                    // }

                    // window->renderer->Println();
                    // window->renderer->Println();

                    // window->renderer->Println("TIME: {} s", to_string(PIT::TimeSinceBoot), Colors.bred);
                    // window->renderer->Println("TIME: {} ms", to_string((int)(PIT::TimeSinceBoot*1000)), Colors.bred);
                    // window->renderer->Println("DIV:  {}", to_string(PIT::Divisor), Colors.bred);
                    // window->renderer->Println("FREQ: {} Hz", to_string(PIT::freq), Colors.bred);




                }
            }
            else
                LogError("You cannot sleep less than 0ms!", window);
        }
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        free(data);
        return;
    }

    if (StrEquals(data->data[0], "set"))
    {
        if (data->len == 3)
            SetCmd(data->data[1], data->data[2], user, window);
        else if (data->len == 2 && StrEquals(data->data[1], "password"))
            SetCmd(data->data[1], "", user, window);
        else
            LogInvalidArgumentCount(2, data->len-1, window);
        
        free(data);
        return;
    }

    if (StrEquals(data->data[0], "get"))
    {
        if (data->len == 2)
            GetCmd(data->data[1], *user, window);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        free(data);
        return;
    }


    if (StrEquals(data->data[0], "login"))
    {
        if (data->len == 2)
            login(data->data[1], user, window);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        free(data);
        return;
    }


    LogError("Unknown command \"{}\"!", data->data[0], window);
    free(data);
    return;
}

void login(const char* name, OSUser** user, Window* window)
{
    OSUser* usr = getUser(name);
    if (usr == 0)
    {
        LogError("User \"{}\" was not found!", name, window);
        return;
    }

    if (StrEquals(usr->password, ""))
        *user = usr;
    else
    {
        window->renderer->Println("Please enter the password down below:");
        (*user)->mode = commandMode::enterPassword;
    }
}

void login(const char* name, const char* pass, OSUser** user, Window* window)
{
    (*user)->mode = commandMode::none;

    OSUser* usr = getUser(name);
    if (usr == 0)
    {
        LogError("User \"{}\" was not found!", name, window);
        return;
    }

    if (StrEquals(usr->password, pass))
        *user = usr;
    else
        LogError("Password is incorrect!", name, window); 
}

void SetCmd(const char* name, const char* val, OSUser** user, Window* window)
{
    if (StrEquals(name, "user color"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            (*user)->colData.userColor = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "default color"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            window->renderer->color = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "username"))
    {
        (*user)->userName = StrCopy(val);
    }
    else if (StrEquals(name, "password"))
    {
        (*user)->mode = commandMode::enterPassword;
        window->renderer->Println("Please enter the new password down below:");
    }
    else if (StrEquals(name, "mouse color front"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            mouseColFront = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "mouse color back"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            mouseColBack = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else
    {
        LogError("Parameter \"{}\" does not exist.", name, window);
    }
}



void GetCmd(const char* name, OSUser* user, Window* window)
{
    if (StrEquals(name, "free ram"))
    {
        window->renderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    }
    else if (StrEquals(name, "free pages"))
    {
        window->renderer->Println("Free Page Count: {} pages.", to_string(GlobalAllocator->GetFreePageCount()), Colors.bgreen);
    }
    else if (StrEquals(name, "window resolution"))
    {
        dispVar vars[] = {dispVar(window->size.width), dispVar(window->size.height)};
        window->renderer->Println("Window Resolution: {0}x{1}.", vars);
    }
    else if (StrEquals(name, "screen resolution"))
    {
        dispVar vars[] = {dispVar(osData.realMainWindow->size.width), dispVar(osData.realMainWindow->size.height)};
        window->renderer->Println("Screen Resolution: {0}x{1}.", vars);
    }
    else
    {
        LogError("Parameter \"{}\" does not exist.", name, window);
    }

    // window->renderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    // window->renderer->Println("");
}

ParsedColData ParseColor(const char* col)
{
    ParsedColData data = ParsedColData();
    data.parseSuccess = false;
    data.col = 0;

    //window->renderer->Println("Col: \"{}\" (", col, Colors.orange);
    //window->renderer->Print(col[0]);
    //window->renderer->Println(")", Colors.orange);

    if (col[0] == '#')
    {
        const char* subStr = StrSubstr(col, 1);
        //window->renderer->Println("Col: \"{}\"", subStr, Colors.cyan);
        data.col = ConvertStringToHex(subStr);
        data.parseSuccess = true;
        return data;
    }

    for (int i = 0; i < colCount; i++)
        if (StrEquals(col, colNames[i]))
        {
            data.col = colValues[i];
            //window->renderer->Println("Color 1: {}", ConvertHexToString(colValues[i]), Colors.white);
            data.parseSuccess = true;
            break;
        }
    
    //window->renderer->Println("Color 2: {}", ConvertHexToString(data.col), Colors.white);

    return data;
}

/*
    //window->renderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);
    StringArrData* data = SplitLine(input);
    //window->renderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);


    window->renderer->Println("Parts:");
    for (int i = 0; i < data->len; i++)
        window->renderer->Println(" - \"{}\"", data->data[i], Colors.bgreen);


    //free(splitLine);
    GlobalAllocator->FreePage(data);
    //window->renderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);
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

    int totalsize = sizeof(StringArrData);
    for (int i = 0; i < partCount; i++)
    {
        totalsize += parts[i] + 1 + sizeof(char*);
    }

    //uint64_t datAddr = (uint64_t) GlobalAllocator->RequestPage();
    int64_t datAddr = (uint64_t) malloc(totalsize);
    StringArrData* data = (StringArrData*) datAddr;

    //char** splitLine = (char**) GlobalAllocator->RequestPage(); //(char**)calloc(partCount, sizeof(char*));

    //window->renderer->Println("ADDR OG:      {}",ConvertHexToString((uint64_t)data),Colors.cyan);

    //window->renderer->Println("ADDR datAddr: {}",ConvertHexToString(datAddr),Colors.bgreen);

    data->addrOfData = (void*)(datAddr + sizeof(StringArrData));

    data->data = (char**)data->addrOfData;


    //window->renderer->Println("ADDR of Data 1: {}",ConvertHexToString((uint64_t)data->data),Colors.cyan);
    //window->renderer->Println("ADDR of Data 2: {}",ConvertHexToString((uint64_t)data->addrOfData),Colors.cyan);

    char** splitLine = data->data;
    data->len = partCount;

    // window->renderer->Println("ADDR 1: {}", ConvertHexToString((uint64_t)splitLine), Colors.yellow);

    // window->renderer->Println("ADDR 2: {}", ConvertHexToString((uint64_t)splitLine), Colors.yellow);

    uint64_t splitIndex = sizeof(char**) + sizeof(char*) * partCount;

    //window->renderer->Println("ADDR of Data 3: {}",ConvertHexToString((uint64_t)data->addrOfData),Colors.cyan);

    for (int i = 0; i < partCount; i++)
    {
        //window->renderer->Println("INDEX  {}", to_string((uint64_t)splitIndex), Colors.cyan); 
        //window->renderer->Println("Count: {}", to_string((uint64_t)parts[i]), Colors.cyan);
        splitLine[i] = (char*)((uint64_t)data->addrOfData + splitIndex);
        //window->renderer->Println("ADDR 1: {}", ConvertHexToString((uint64_t)&splitLine[i][0]), Colors.yellow);

        for (int i2 = 0; i2 < parts[i] + 1; i2++)
            splitLine[i][i2] = 0;
       
        //window->renderer->Println("ADDR 2: {}", ConvertHexToString((uint64_t)splitLine[i]), Colors.yellow);
        //window->renderer->Println("ADDR 3: {}", ConvertHexToString((uint64_t)data->addrOfData + splitIndex), Colors.yellow);
        splitIndex += parts[i] + 1;
    }

    // ---------------------------------------------------------------------------------------------------------------------------------

    inString = false;
    partIndex = 0;
    index = 0;

    for (int i = 0; i < 100; i++)
        parts[i] = 0;
    
    //window->renderer->Println("AAA");
    //window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
    //window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);

    //window->renderer->Println("Lines:", Colors.yellow);

    for (; input[index] != 0; index++)
    {
        if (input[index] == '\"')
            inString = !inString;
        else if (!inString && input[index] == ' ')
        {
            //window->renderer->Println();
            //window->renderer->Println("PartIndex:   {}", to_string(partIndex), Colors.cyan);
            //window->renderer->Println("PartIndex 2: {}", to_string(parts[partIndex]), Colors.cyan);
            //window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
            //window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);
            splitLine[partIndex][parts[partIndex]] = 0;
            //window->renderer->Println(" - \"{}\"", (const char*)splitLine[partIndex], Colors.yellow);
            //window->renderer->Println(", Count: {}", to_string((uint64_t)parts[partIndex] + 1), Colors.yellow);
            partIndex++;
        }
        else 
        {
            if (input[index] == '\\')
                if (input[index + 1] != 0)
                    index++;
            
            //splitLine[partIndex][parts[partIndex]] = input[index];
            //*(splitLine[partIndex] + parts[partIndex]) = input[index];
            //window->renderer->Print(input[index]);
            //window->renderer->Println("Index:           {}", to_string(index), Colors.bgreen);
            //window->renderer->Println("Part Index:      {}", to_string(partIndex), Colors.bgreen);
            //window->renderer->Println("Part Part Index: {}", to_string(parts[partIndex]), Colors.bgreen);

            splitLine[partIndex][parts[partIndex]] = input[index];

            parts[partIndex]++;
        }
    }

    ///window->renderer->Println(" - \"{}\"", (const char*)splitLine[partIndex], Colors.yellow);
    // window->renderer->Println();
    // window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
    // window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);
    // window->renderer->Print(" - \"{}\"", splitLine[partIndex], Colors.yellow);
    // window->renderer->Println(", Count: {}", to_string((uint64_t)parts[partIndex] + 1), Colors.yellow);


    return data;
}