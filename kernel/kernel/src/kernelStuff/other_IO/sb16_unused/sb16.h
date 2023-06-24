#pragma once
#include <stdbool.h>
#include <stdint.h>

namespace SB16
{
    extern bool SB16Initialized;

    void SB16Timeout();

    bool SB16ResetDSP();

    void SB16SetIRQ();

    void SB16SetBuff(uint32_t Addr, uint16_t Len);

    void SB16Program();

    bool SB16Init();

}