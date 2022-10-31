#pragma once

#include <stdint.h>
#include <stddef.h>

namespace MAAB_MEM
{

    const uint32_t MbMagicNum = 54135325;

    struct MbSegHdr
    {
        size_t length;
        MbSegHdr* next;
        MbSegHdr* last;
        void* memHandler;
        bool free;
        void CombineForward();
        void CombineBackward();
        MbSegHdr* Split(size_t splitLength);
        uint32_t magicNum;
    };

    class MbMemHandler
    {   

        

    public:
        MbMemHandler(void* bufAddr, uint64_t size, uint64_t _trueOffset);
        void* MallocMem(size_t size);
        bool FreeMem(void* address);

        MbSegHdr* lastHdr;
        void* buffer;
        uint64_t bufSize;
        uint64_t trueOffset;
    };


}