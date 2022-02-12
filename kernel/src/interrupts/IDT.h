#pragma once
#include <stdint.h>

#define IDT_TA_InterruptGate 0b10001110
#define IDT_TA_CallGate      0b10001100
#define IDT_TA_TrapGqate     0b10001111

struct IDTDescEntry
{
    uint16_t offset0;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset1;
    uint32_t offset2;
    uint32_t ignore;

    void SetOffset(uint64_t offset)
    {
        offset0 = (uint16_t) (offset & 0x000000000000ffff);
        offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
        offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);
    }

    uint64_t GetOffset()
    {
        uint64_t offset = 0;
        offset |= (uint64_t)offset0;
        offset |= (uint64_t)offset1 << 16;
        offset |= (uint64_t)offset2 << 32;
        return offset;
    }


};

struct IDTR
{
    uint16_t Limit;
    uint64_t Offset;
}__attribute__((packed));
