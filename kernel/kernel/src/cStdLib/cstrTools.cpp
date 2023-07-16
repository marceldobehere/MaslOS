#include "cstrTools.h"
#include "../paging/PageFrameAllocator.h"
#include "../memory/heap.h"
#include "../osData/MStack/MStackM.h"

bool StrEquals(const char* a, const char* b)
{
    AddToStack();
    int diff = StrLen(a) - StrLen(b);
    if (diff != 0)
    {
        RemoveFromStack();
        return false;
    }
    RemoveFromStack();

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

bool StrEquals(const char* a, const char* b, int len)
{
    AddToStack();
    if (a == NULL || b == NULL)
    {
        RemoveFromStack();
        return false;
    }

    for (int index = 0; index < len; index++)
    {
        if (a[index] != b[index])
        {
            RemoveFromStack();
            return false;
        }
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
    char* newStr = (char*)_Malloc(size + 1, "strcopy");

    for (int i = 0; og[i] != 0; i++)
        newStr[i] = og[i];
    newStr[size] = 0;
    RemoveFromStack();
    return newStr;
}

char* StrAppend(const char* a, const char* b, bool freeA)
{
    AddToStack();
    int sizeA = StrLen(a);
    int sizeB = StrLen(b);

    char* newStr = (char*)_Malloc(sizeA + sizeB + 1, "strappend");

    for (int i = 0; i < sizeA; i++)
        newStr[i] = a[i];
    for (int i = 0; i < sizeB; i++)
        newStr[i + sizeA] = b[i];
    newStr[sizeA + sizeB] = 0;

    if (freeA)
        _Free((void*)a);
    RemoveFromStack();
    return newStr;
}

char* StrPadLeft(const char* a, char pad, int totalLen, bool freeA)
{
    AddToStack();
    int size = StrLen(a);
    if (size >= totalLen)
    {
        if (freeA)
            _Free((void*)a);

        RemoveFromStack();
        return StrCopy(a);
    }

    char* newStr = (char*)_Malloc(totalLen + 1, "strpadleft");
    for (int i = 0; i < totalLen - size; i++)
        newStr[i] = pad;
    for (int i = 0; i < size; i++)
        newStr[i + totalLen - size] = a[i];
    newStr[totalLen] = 0;

    if (freeA)
        _Free((void*)a);
    RemoveFromStack();
    return newStr;
}

char* StrPadRight(const char* a, char pad, int totalLen, bool freeA)
{
    AddToStack();
    int size = StrLen(a);
    if (size >= totalLen)
    {
        if (freeA)
            _Free((void*)a);

        RemoveFromStack();
        return StrCopy(a);
    }

    char* newStr = (char*)_Malloc(totalLen + 1, "strpadright");
    for (int i = 0; i < size; i++)
        newStr[i] = a[i];
    for (int i = 0; i < totalLen - size; i++)
        newStr[i + size] = pad;
    newStr[totalLen] = 0;

    if (freeA)
        _Free((void*)a);
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
    char* newStr = (char*)_Malloc(len + 1, "substring");
    for (int i = 0; i < len; i++)
        newStr[i] = og[i+index];
    newStr[len] = 0;
    RemoveFromStack();
    return newStr;
}

int32_t StrLen(const char* og)
{
    int32_t len;
    for (len = 0; og[len] != 0; len++);
    return len;
}

char* StrSubstr(const char* og, int index)
{
    AddToStack();
    int len = StrLen(og);//GlobalRenderer->Print(og[len]);

    //GlobalRenderer->Println("\n\rLEN 0: {}", to_string((uint64_t)len), Colors.white);

    if (index < 0)
        index = 0;

    if (index >= len)
        index = len;

    len -= index;

    int length = len;


    char* res = StrSubstr(og, index, len);
    RemoveFromStack();
    return res;
}

bool StrStartsWith(const char* og, const char* with)
{
    int diff = StrLen(og) - StrLen(with);
    if (diff < 0)
        return false;
    
    for (;*with != 0; og++, with++)
        if (*og != *with)
            return false;

    return true;
}

bool StrEndsWith(const char* og, const char* with)
{
    if (og == NULL || with == NULL)
        return false;
    int diff = StrLen(og) - StrLen(with);
    if (diff < 0)
        return false;
    
    og += diff;
    for (;*with != 0; og++, with++)
        if (*og != *with)
            return false;

    return true;
}

char* StrCombine(const char* a, const char* b)
{
    int lenA = StrLen(a);
    int lenB = StrLen(b);
    
    int totalLen = lenA + lenB;
    char* tempStr = (char*) _Malloc(totalLen + 1, "Malloc for Combined String");
    tempStr[totalLen] = 0;

    for (int i = 0; i < lenA; i++)
        tempStr[i] = a[i];
    for (int i = 0; i < lenB; i++)
        tempStr[i + lenA] = b[i];

    return tempStr;
}

char* StrCombine(const char* a, const char* b, const char* c)
{
    int lenA = StrLen(a);
    int lenB = StrLen(b);
    int lenC = StrLen(c);
    
    int totalLen = lenA + lenB + lenC;
    char* tempStr = (char*) _Malloc(totalLen + 1, "Malloc for Combined String");
    tempStr[totalLen] = 0;

    for (int i = 0; i < lenA; i++)
        tempStr[i] = a[i];
    for (int i = 0; i < lenB; i++)
        tempStr[i + lenA] = b[i];
    for (int i = 0; i < lenC; i++)
        tempStr[i + lenA + lenB] = c[i];

    return tempStr;
}

char* StrCombine(const char* a, const char* b, const char* c, const char* d)
{
    int lenA = StrLen(a);
    int lenB = StrLen(b);
    int lenC = StrLen(c);
    int lenD = StrLen(d);
    
    int totalLen = lenA + lenB + lenC + lenD;
    char* tempStr = (char*) _Malloc(totalLen + 1, "Malloc for Combined String");
    tempStr[totalLen] = 0;

    for (int i = 0; i < lenA; i++)
        tempStr[i] = a[i];
    for (int i = 0; i < lenB; i++)
        tempStr[i + lenA] = b[i];
    for (int i = 0; i < lenC; i++)
        tempStr[i + lenA + lenB] = c[i];
    for (int i = 0; i < lenD; i++)
        tempStr[i + lenA + lenB + lenC] = d[i];

    return tempStr;
}



uint64_t StrCountChr(const char* og, char chr)
{
    uint64_t count = 0;

    for (; *og != 0; og++)
        if (*og == chr)
            count++;

    return count;
}

char* StrReplaceStartingStuffWith(const char* og, const char* toReplace, const char* toReplaceWith)
{
    if (!StrStartsWith(og, toReplace))
        return StrCopy(og);
    
    int len = StrLen(toReplace);
    if (StrEquals(og, toReplace))
        return StrCopy(toReplaceWith);

    char* subStr = StrSubstr(og, len);
    char* combined = StrCombine(toReplaceWith, subStr);
    _Free(subStr);
    return combined;
}

int32_t StrIndexOf(const char* str, char chr)
{
    for (int i = 0; str[i] != 0; i++)
        if (str[i] == chr)
            return i;
    return -1;
}

int32_t StrLastIndexOf(const char* str, char chr)
{
    int lastI = -1;
        for (int i = 0; str[i] != 0; i++)
        if (str[i] == chr)
            lastI = i;
    return lastI;
}


int32_t StrIndexOf(const char* str, char chr, int ignoreCount)
{
    for (int i = 0; str[i] != 0; i++)
        if (str[i] == chr)
            if (--ignoreCount < 0)
                return i;
    return -1;
}

int32_t StrLastIndexOf(const char* str, char chr, int ignoreCount)
{
    int len = StrLen(str);

    for (int i = len-1; i >= 0; i--)
        if (str[i] == chr)
            if (--ignoreCount < 0)
                return i;
    return -1;
}