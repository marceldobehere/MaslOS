#pragma once
#include <stdint.h>

enum PT_Flag
{
    Present = 0,
    ReadWrite = 1,
    UserSuper = 2,
    WriteThrough = 3,
    CacheDisabled = 4,
    Accessed = 5,
    LargerPages = 7,
    Custom0 = 9,
    Custom1 = 10,
    Custom2 = 11,
    NX = 63 // only if supported
};


struct PageDirectoryEntry
{
    uint64_t Value;
    void SetFlag(PT_Flag flag, bool enabled)
    {
        uint64_t bitSelector = (uint64_t) 1 << flag;
        Value &= ~bitSelector;
        if (enabled)
            Value |= bitSelector;
    }
    bool GetFlag(PT_Flag flag)
    {
        uint64_t bitSelector = (uint64_t) 1 << flag;
        return ((Value & bitSelector) > 0);
    }
    void SetAddress(uint64_t address)
    {
        address &= 0x000000ffffffffff;
        Value &= 0xfff0000000000fff;
        Value |= (address << 12);
    }
    uint64_t GetAddress()
    {
        return (Value & 0x000ffffffffff000) >> 12;
    }
};



struct PageTable
{
    PageDirectoryEntry entries [512];
    

}__attribute__((aligned(0x1000)));