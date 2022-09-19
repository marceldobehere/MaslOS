#pragma once
#include <stdint.h>

namespace PIT
{
    extern uint64_t TicksSinceBoot;
    extern uint64_t freq;
    extern uint16_t Divisor;
    static const uint64_t BaseFrequency = 1193182;
    
    void Sleepd(double seconds);
    void Sleep(uint64_t milliseconds);
    void InitPIT();

    void SetDivisor(uint16_t divisor);
    double TimeSinceBootS();
    uint64_t TimeSinceBootMS();
    uint64_t TimeSinceBootMicroS();
    uint64_t GetFrequency();
    void SetFrequency(uint64_t frequency);
    void Tick();
}
