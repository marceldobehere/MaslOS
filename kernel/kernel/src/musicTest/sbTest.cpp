#include "sbTest.h"


namespace MusicBit16Test
{
    void FillArray(uint16_t* buf, int start, int len, int freq, int sampleRate)
    {
        // sample rate is 48000 Hz

        int maxQ = sampleRate / freq;
        int halfQ = maxQ / 2;

        for (int i = start; i < start + len; i++)
        {
            int tI = i % maxQ - halfQ;
            // if (tI < 0)
            //     tI = -tI;
            buf[i] = (uint16_t)((0x1000 * tI) / halfQ);

            // if (i % maxQ < halfQ)
            //     buf[i] = 0x1000;
            // else
            //     buf[i] = 0x0000;
        }
    }
}