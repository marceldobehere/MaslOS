#include "../../IO/IO.h"
#include "sb16.h"



namespace SB16
{
    bool SB16Initialized = false;

    void SB16Timeout()
    {
        for (int i = 0; i < 100;i++)
        {
            io_wait();
        }
    }

    bool SB16ResetDSP()
    {
        outb(0x226, 1);
        SB16Timeout();
        outb(0x226, 0);
        return inb(0x22A) == 0xAA;
    }

    void SB16SetIRQ()
    {
        outb(0x224, 0x80);
        outb(0x225, 0x08); // IRQ 10
    }


    void SB16SetBuff(uint32_t Addr, uint16_t Len)
    {
        Len -= 1; // prolly needed igggg
        Addr &= 0x00FFFFFF;
        outb(0xD4, 5); // Disable channel
        outb(0xD8, 1); // Flip flop
        outb(0xD6, 0x59); // (0x48 for single mode, 0x58 for auto mode) + channel number
        outb(0x8B, Addr >> 12); // Send page number of addr
        outb(0xC4, Addr & 0xFF); // Send low bits of addr
        outb(0xC4, (Addr >> 8) & 0xFF); // Send middle bits of addr
        outb(0xC6, Len & 0xFF); // Send low bits of len
        outb(0xC6, (Len >> 8) & 0xFF); // Send high bits of len
        outb(0xD4, 1); // Finally, enable channel
    }

    //TC = 256 - (1000000 / (channel * rate))
    int GetTimeConstant(int channel, int rate)
    {
        return 256 - (1000000 / (channel * rate));
    }

    void SB16Program()
    {
        int tC = GetTimeConstant(1, 11931);
        tC = 172; // should be correct i think


        outb(0x22C, 0x40); // Set time constant
        outb(0x22C, tC); // 10989 Hz
        outb(0x22C, 0xB0); // 16-bit sound
        outb(0x22C, 0b10000); // Mono and signed sound data
        outb(0x22C, 0xFE); // Count low bits
        outb(0x22C, 0xFF); // Count high bits
    }

    bool SB16Init()
    {
        if (!SB16ResetDSP())
        {
            SB16Initialized = false;
            return false;
        }

        SB16SetIRQ();

        outb(0x22C, 0xD1); // Turn speaker on

        SB16Program();

        SB16Initialized = true;
        return true;
    }

}