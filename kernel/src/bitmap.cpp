#include "bitmap.h"
#include <stddef.h>
#include <stdint.h>
#include "OSDATA/MStack/MStackM.h"


size_t Size;
uint8_t* Buffer;

bool Bitmap::operator[](uint64_t index)
{
    AddToStack("operator[]", "bitmap.cpp");
    if (index > Size * 8)
    {
        RemoveFromStack();
        return false;
    }
    uint8_t temp = Buffer[index / 8];
    temp = (temp << (index % 8));
    temp = (temp >> 7);
    RemoveFromStack();
    return temp != 0;
}

bool Bitmap::Set(uint64_t index, bool value)
{
    AddToStack("Set", "bitmap.cpp");
    if (index > Size * 8)
    {
        RemoveFromStack();
        return false;
    }
    uint64_t index2 = index / 8;
    
    uint8_t bitIndexer = 0b10000000;
    bitIndexer = bitIndexer >> (index % 8);
    Buffer[index2] = Buffer[index2] & (~bitIndexer);

    if (value)
        Buffer[index2] = Buffer[index2] | bitIndexer;
        
    RemoveFromStack();
    return true;
}