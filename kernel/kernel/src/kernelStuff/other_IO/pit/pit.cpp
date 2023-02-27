#include "pit.h"
#include "../../IO/IO.h"
#include "../../../OSDATA/osdata.h"

namespace PIT
{
    uint64_t TicksSinceBoot = 0;

    uint16_t Divisor = 65535;

    bool Inited = false;

    uint64_t freq = GetFrequency();

    void InitPIT()
    {
        TicksSinceBoot = 0;
        SetDivisor(400 /*65535*/);
        freq = GetFrequency();
        Inited = true;
    }

    void SetDivisor(uint16_t divisor)
    {
        if (divisor < 100)
            divisor = 100;

        Divisor = divisor;
        outb(0x43, 0x36);
        outb(0x40, (uint8_t)(divisor & 0x00ff));
        io_wait();
        outb(0x40, (uint8_t)((divisor & 0xff00) >> 8));
        io_wait();
        freq = GetFrequency();
    }

    void Sleep(uint64_t milliseconds)
    {
        if (osData.NO_INTERRUPTS || !Inited)
            return;
        uint64_t endTime = TimeSinceBootMS() + milliseconds;
        while (TimeSinceBootMS() < endTime)
            asm("hlt");
    }

    void Sleepd(double seconds)
    {
        if (osData.NO_INTERRUPTS)
            return;
        Sleepd((uint64_t)(seconds * 1000));
    }

    uint64_t GetFrequency()
    {
        return BaseFrequency / Divisor;
    }

    void SetFrequency(uint64_t frequency)
    {
        SetDivisor(BaseFrequency / frequency);
        freq = GetFrequency();
    }

    void Tick()
    {
        TicksSinceBoot++;
    }

    double TimeSinceBootS()
    {
        return (TicksSinceBoot)/(double)freq;
    }
    
    uint64_t TimeSinceBootMS()
    {
        return (TicksSinceBoot*1000)/freq;
    }

    uint64_t TimeSinceBootMicroS()
    {
        return (TicksSinceBoot*1000000)/freq;
    }

}