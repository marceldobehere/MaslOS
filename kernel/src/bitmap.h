#pragma once
#include <stddef.h>

class Bitmap
{
   public:
        size_t Size;
        uint8_t* Buffer;

        bool operator[](uint64_t index)
        {
            uint8_t temp = Buffer[index / 8];
            temp = (temp << (index % 8));
            temp = (temp >> 7);
            return temp != 0;
        }

        void Set(uint64_t index, bool value)
        {
            uint64_t index2 = index / 8;
            
            uint8_t bitIndexer = 0b10000000;
            bitIndexer = bitIndexer >> (index % 8);
            Buffer[index2] = Buffer[index2] & (~bitIndexer);

            if (value)
                Buffer[index2] = Buffer[index2] | bitIndexer;
        }
};