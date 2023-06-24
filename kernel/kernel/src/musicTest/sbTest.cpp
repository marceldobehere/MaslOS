#include "sbTest.h"


namespace MusicBit16Test
{
    void FillArray(uint16_t* buf, int start, int len, int freq)
    {
        // sample rate is 10000 Hz

        int maxQ = 10000 / freq;
        int halfQ = maxQ / 2;

        for (int i = start; i < start + len; i++)
        {
            if (i % maxQ < halfQ)
                buf[i] = 0x0FFF;
            else
                buf[i] = 0x0000;
        }
    }
}