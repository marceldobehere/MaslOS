#include "cstrTools.h"
#include "../paging/PageFrameAllocator.h"
#include "../memory/heap.h"
#include "../OSDATA/MStack/MStackM.h"

bool StrEquals(const char* a, const char* b)
{
    AddToStack();
    int index = 0;
    while (!(a[index] == 0 && b[index] == 0))
    {
        if (a[index] != b[index])
        {
            RemoveFromStack();
            return false;
        }
        index++;
    }
    RemoveFromStack();
    return true;
}

char* StrCopy(const char* og)
{
    AddToStack();
    int size = 0;
    for (int i = 0; og[i] != 0; i++)
        size++;
    char* newStr = (char*)malloc(size + 1);

    for (int i = 0; og[i] != 0; i++)
        newStr[i] = og[i];
    newStr[size] = 0;
    RemoveFromStack();
    return newStr;
}

char* StrSubstr(const char* og, int index, int len)
{
    AddToStack();

    if (index < 0)
        index = 0;

    int size = 0;
    for (size = 0; og[size] != 0; size++);

    if (index >= size)
        index = size;

    if (len + index > size)
        len = size - index;

    if (len < 0)
        len = 0;

    //GlobalRenderer->Print("LEN: {}  ", to_string((uint64_t)len), Colors.white);
    char* newStr = (char*)malloc(len + 1);
    for (int i = 0; i < len; i++)
        newStr[i] = og[i+index];
    newStr[len] = 0;
    RemoveFromStack();
    return newStr;
}

char* StrSubstr(const char* og, int index)
{
    //AddToStack();
    int len;
    for (len = 0; og[len] != 0; len++);//GlobalRenderer->Print(og[len]);

    //GlobalRenderer->Println("\n\rLEN 0: {}", to_string((uint64_t)len), Colors.white);

    if (index < 0)
        index = 0;

    if (index >= len)
        index = len;

    len -= index;

    int length = len;


    {
        char* res = StrSubstr(og, index, len);
        RemoveFromStack();
        return res;
    }

    // GlobalRenderer->Println("LEN 1: {}", to_string((uint64_t)length), Colors.white);
    // // char* res = StrSubstr(og, index, len);
    
    // // for (len = 0; res[len] != 0; len++);
    // // GlobalRenderer->Println("LEN 2: {}", to_string((uint64_t)len), Colors.white);

    // GlobalRenderer->Println("ADDR 1: {}", ConvertHexToString((uint64_t)og), Colors.white);
    
    // char* res = (char*)malloc(length + 1);
    // for (int i = 0; i < length; i++)
    //     res[i] = og[i+index];
    // res[length] = 0;

    // GlobalRenderer->Println("ADDR 2: {}", ConvertHexToString((uint64_t)og), Colors.white);
    // GlobalRenderer->Println("ADDR 3: {}", ConvertHexToString((uint64_t)res), Colors.white);
    // for (len = 0; og[len] != 0; len++)GlobalRenderer->Print(og[len]);
    // GlobalRenderer->Println();

    

    // for (len = 0; res[len] != 0; len++)GlobalRenderer->Print(res[len]);
    // GlobalRenderer->Println();



    // RemoveFromStack();
    // return res;
}

