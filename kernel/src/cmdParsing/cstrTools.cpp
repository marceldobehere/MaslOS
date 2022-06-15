#include "cstrTools.h"
#include "../paging/PageFrameAllocator.h"
#include "../memory/heap.h"
#include "../OSDATA/MStack/MStackM.h"

bool StrEquals(const char* a, const char* b)
{
    AddToMStack(MStack("StrEquals", "cmdParsing/cstrTools.cpp"));
    int index = 0;
    while (!(a[index] == 0 && b[index] == 0))
    {
        if (a[index] != b[index])
        {
            RemoveLastMStack();
            return false;
        }
        index++;
    }
    RemoveLastMStack();
    return true;
}

char* StrCopy(const char* og)
{
    AddToMStack(MStack("StrCopy", "cmdParsing/cstrTools.cpp"));
    int size = 0;
    for (int i = 0; og[i] != 0; i++)
        size++;
    char* newStr = (char*)malloc(size + 1);

    for (int i = 0; og[i] != 0; i++)
        newStr[i] = og[i];
    newStr[size] = 0;
    RemoveLastMStack();
    return newStr;
}

char* StrSubstr(const char* og, int index, int len)
{
    AddToMStack(MStack("StrSubstr", "cmdParsing/cstrTools.cpp"));
    int size = 0;
    for (int size = 0; og[size] != 0; size++);

    if (index >= size)
        index = size - 1;

    if (len + index > size)
        len = size - index;

    //GlobalRenderer->Print("LEN: {}  ", to_string((uint64_t)len), Colors.white);
    char* newStr = (char*)malloc(len + 1);
    for (int i = 0; i < len; i++)
        newStr[i] = og[i+index];
    newStr[len] = 0;
    RemoveLastMStack();
    return newStr;
}

char* StrSubstr(const char* og, int index)
{
    AddToMStack(MStack("StrSubstr", "cmdParsing/cstrTools.cpp"));
    int len;
    for (len = 0; og[len] != 0; len++)GlobalRenderer->Print(og[len]);

    GlobalRenderer->Println("\n\rLEN 0: {}", to_string((uint64_t)len), Colors.white);

    if (index >= len)
        index = len - 1;

    len -= index;

    GlobalRenderer->Println("LEN 1: {}", to_string((uint64_t)len), Colors.white);
    // char* res = StrSubstr(og, index, len);
    
    // for (len = 0; res[len] != 0; len++);
    // GlobalRenderer->Println("LEN 2: {}", to_string((uint64_t)len), Colors.white);

    GlobalRenderer->Println("ADDR 1: {}", ConvertHexToString((uint64_t)og), Colors.white);
    char* res = (char*)malloc(len + 1);
    GlobalRenderer->Println("ADDR 2: {}", ConvertHexToString((uint64_t)og), Colors.white);
    GlobalRenderer->Println("ADDR 3: {}", ConvertHexToString((uint64_t)res), Colors.white);
    for (len = 0; og[len] != 0; len++)GlobalRenderer->Print(og[len]);
    GlobalRenderer->Println();

    

    for (int i = 0; i < len; i++)
        res[i] = og[i+index];
    res[len] = 0;

    for (len = 0; res[len] != 0; len++)GlobalRenderer->Print(res[len]);
    GlobalRenderer->Println();



    RemoveLastMStack();
    return res;
}

