#include "audio.h"
#include "../memory/heap.h"
#include "../cStdLib/cstrTools.h"
#include "../cStdLib/cstr.h"
#include "../interrupts/panic.h"
#include "..//devices/serial/serial.h"

namespace Audio
{

    void SampleConvertAndAdd(void* dest, int destIndx, int destBPS, int destCC, void* src, int srcIndx, int srcBPS, int srcCC, int srcVol)
    {
        // use channel count (cc) as the multiplier for the index
        // convert from src bits per second (bps) at src index to dest bps at dest index

        // Add Averaging if SrcCC > DestCC
        // Example Stereo (Src) to Mono (Dest)
        
        for (int destTC = 0, srcTC = 0; destTC < destCC; destTC++, srcTC++)
        {
            if (srcTC >= srcCC)
                srcTC = 0;
            // convert from src bps to 32 bit
            int32_t srcSample = 0;
            long srcIndex = ((srcIndx * srcCC + srcTC) * srcBPS) / 8;
            switch (srcBPS)
            {
                case 8:
                {
                    //srcSample = ((int8_t*)src)[srcIndex];
                    int8_t t8 = *((int8_t*)((uint64_t)src + srcIndex));
                    srcSample = t8;
                    break;
                }
                case 16:
                {
                    //srcSample = ((int16_t*)src)[srcIndex];
                    int16_t t16 = *((int16_t*)((uint64_t)src + srcIndex));
                    srcSample = t16;
                    break;
                }
                case 32:
                {
                    //srcSample = ((int32_t*)src)[srcIndex];
                    int32_t t32 = *((int32_t*)((uint64_t)src + srcIndex));
                    srcSample = t32;
                    break;                    
                }
                default:
                    Panic("Invalid Source BPS {}", to_string(srcBPS), true);
                    break;
            }
            
            srcSample = (srcSample * srcVol) / 100;

            // deal with changing bits per second messing with the actual audio data/volume bc ranges are different
            if (srcBPS == 16 && destBPS == 8)
            {
                uint16_t sixteenBit = (uint16_t)srcSample;
                uint8_t eightBit = sixteenBit >> 8;
                eightBit += eightBit < 0xff && ((sixteenBit & 0xff) > 0x80);
                srcSample = eightBit;
            }
            else if (srcBPS == 8 && destBPS == 16)
            {
                uint16_t sixteenBit = 0;
                uint8_t eightBit = (uint8_t)srcSample;
                if (eightBit & 0x80)
                    sixteenBit = 0xff00 | eightBit;
                else
                    sixteenBit = eightBit;
                srcSample = sixteenBit;
            }

            long destIndex = ((destIndx * destCC + destTC) * destBPS) / 8;
            // convert from 32 bit to dest bps
            switch (destBPS)
            {
                case 8:
                    //((int8_t*)dest)[destIndex] += (int8_t)srcSample;
                    *((int8_t*)((uint64_t)dest + destIndex)) += (int8_t)srcSample;
                    break;
                case 16:
                    //((int16_t*)dest)[destIndex] += (int16_t)srcSample;
                    *((int16_t*)((uint64_t)dest + destIndex)) += (int16_t)srcSample;
                    break;
                case 32:
                    //((int32_t*)dest)[destIndex] += (int32_t)srcSample;
                    *((int32_t*)((uint64_t)dest + destIndex)) += (int32_t)srcSample;
                    break;
                default:
                    Panic("Invalid Destination BPS {}", to_string(destBPS), true);
                    break;
            }
        }
    }

    AudioBuffer::AudioBuffer(int bitsPerSample, int sampleRate, int channelCount, int sampleCount)
    {
        this->bitsPerSample = bitsPerSample;
        this->sampleRate = sampleRate;
        this->channelCount = channelCount;
        this->totalSampleCount = sampleCount;
        this->sampleCount = 0;
        this->volume = 100;
        this->byteCount = ((long)bitsPerSample * (long)channelCount * (long)totalSampleCount) / 8;

        this->data = (void*)_Malloc(byteCount, "AudioBuffer");
        ClearBuffer();
    }
    void AudioBuffer::ClearBuffer()
    {
        uint8_t* data = (uint8_t*)this->data;
        for (int i = 0; i < byteCount; i++)
            data[i] = 0;
        sampleCount = 0;
    }
    void AudioBuffer::Free()
    {
        _Free(this->data);
        _Free(this);
    }
    int AudioBuffer::MixBuffer(AudioBuffer* other, int sampleOffset)
    {
        // Try to mix the other buffer into the current one
        // Tries to convert correct Samplerate and everything
        // Will return amount of samples read from the other buffer

        long sC1 = sampleCount;
        long sC2 = other->sampleCount - sampleOffset;
        if (sC2 <= 0)
            return 0;
        
        long sR1 = sampleRate;
        long sR2 = other->sampleRate;
        if (sR1 == 0 || sR2 == 0)
            return 0;

        //float sR3 = sR1 / (float)sR2; // x Samples of other to 1 sample of this
        long asC2 = ((sC2 * sR1) / sR2); // amount of samples of other to mix into this
        //Panic("YO {}", to_string(sC2 * sR1), true);
        long commonSC = min(sC1, asC2); // amount of samples to mix
        if (commonSC <= 0)
            return sC2;
        
        long osC2 = ((commonSC * sR2 + (sR1 / 2)) / sR1); // amount of samples of this to mix into other


        int cC1 = channelCount;
        int cC2 = other->channelCount;

        int bPS1 = bitsPerSample;
        int bPS2 = other->bitsPerSample;

        int v = other->volume;



        // Serial::Writeln("SC1: {}", to_string(sC1));
        // Serial::Writeln("SC2: {}", to_string(sC2));
        // Serial::Writeln();
        // Serial::Writeln("SR1: {}", to_string(sR1));
        // Serial::Writeln("SR2: {}", to_string(sR2));
        // Serial::Writeln();
        // Serial::Writeln("AS2: {}", to_string(asC2));
        // Serial::Writeln();
        // Serial::Writeln("CSC: {}", to_string(commonSC));
        // Serial::Writeln("OSC: {}", to_string(osC2));
        // Serial::Writeln();
        // Serial::Writeln("CC1: {}", to_string(cC1));
        // Serial::Writeln("CC2: {}", to_string(cC2));
        // Serial::Writeln();
        // Serial::Writeln("BPS1: {}", to_string(bPS1));
        // Serial::Writeln("BPS2: {}", to_string(bPS2));
        // Serial::Writeln();
        // Serial::Writeln("V: {}", to_string(v));
        // Serial::Writeln();
        // Serial::Writeln();


        

        for (int i = 0; i < commonSC; i++)
        {
            int sI1 = i;
            int sI2 = ((i * sR2) / sR1) + sampleOffset; //((i / sR3)) + sampleOffset;

            // Serial::Writeln("> I: {}", to_string(i));
            // Serial::Writeln("I1: {}", to_string(sI1));
            // Serial::Writeln("I2: {}", to_string(sI2));
            // Serial::Writeln();

            

            SampleConvertAndAdd(
                (uint8_t*)this->data, sI1, bPS1, cC1, 
                (uint8_t*)other->data, sI2, bPS2, cC2, 
                v);
        }

        //Panic("YO {}", to_string(commonSC), true);

        return osC2;
    }


    AudioBuffer* AudioBuffer::Create16BitMonoBuffer(int sampleRate, int sampleCount)
    {
        return new AudioBuffer(16, sampleRate, 1, sampleCount);
    }
    AudioBuffer* AudioBuffer::Create16Bit48KHzMonoBuffer(int sampleCount)
    {
        return new AudioBuffer(16, 48000, 1, sampleCount);
    }
    AudioBuffer* AudioBuffer::Create16BitStereoBuffer(int sampleRate, int sampleCount)
    {
        return new AudioBuffer(16, sampleRate, 2, sampleCount);
    }
    AudioBuffer* AudioBuffer::Create16Bit48KHzStereoBuffer(int sampleCount)
    {
        return new AudioBuffer(16, 48000, 2, sampleCount);
    }



    BasicAudioDestination::BasicAudioDestination(AudioBuffer* buffer)
    {
        sources = new List<void*>();
        this->buffer = buffer;
        this->buffer->sampleCount = this->buffer->totalSampleCount;
    }
    int BasicAudioDestination::RequestBuffer(BasicAudioSource* from)
    {
        if (buffer == NULL)
            return 0;
        if (from == NULL)
            return 0;
        if (from->buffer == NULL)
            return 0;
        if (!from->readyToSend)
            return 0;
        
        int c = this->buffer->MixBuffer(from->buffer, from->samplesSent);
        from->samplesSent += c;
        
        if (from->samplesSent >= from->buffer->sampleCount)
        {
            from->readyToSend = false;
            from->samplesSent = 0;
            from->buffer->sampleCount = 0;
            // from->buffer->ClearBuffer(); //maybe remove later?

            if (from->OnFinish != NULL)
            {
                from->OnFinish(from->OnFinishHelp, this);
            }

            // Serial::Writeln();
            // Serial::Writeln("FROM SAMPLES SENT: {}", to_string(from->samplesSent));
            // Serial::Writeln("FROM SAMPLES COUNT: {}", to_string(from->buffer->sampleCount));
            // Panic("BRUH? {}", to_string(from->buffer->sampleCount), true);
        }



        return c;
    }

    int BasicAudioDestination::RequestBuffers()
    {
        if (sources == NULL)
            return 0;
        int c = 0;
        for (int i = 0; i < sources->getCount(); i++)
        {
            BasicAudioSource* src = (BasicAudioSource*)sources->elementAt(i);
            c += RequestBuffer(src);
        }
        return c;
    }

    bool BasicAudioDestination::AllBuffersDone()
    {
        if (sources == NULL)
            return true;
        for (int i = 0; i < sources->getCount(); i++)
        {
            BasicAudioSource* src = (BasicAudioSource*)sources->elementAt(i);
            if (src->readyToSend)
                return false;
        }
        return true;
    }

    void BasicAudioDestination::Free()
    {
        if (buffer != NULL)
            buffer->Free();
        buffer = NULL;
        if (sources != NULL)
        {
            sources->free();
            _Free(sources);
        }
        sources = NULL;
        _Free(this);
    }


    BasicAudioSource::BasicAudioSource(AudioBuffer* buffer)
    {
        this->destinations = new List<void*>();
        this->buffer = buffer;
        this->readyToSend = false;
        this->samplesSent = 0;
        OnFinish = NULL;
        OnFinishHelp = NULL;
    }
    void BasicAudioSource::ConnectTo(BasicAudioDestination* dest)
    {
        if (dest == NULL)
            return;
        if (dest->sources == NULL)
            return;
        dest->sources->add(this);
        destinations->add((void*)dest);
    }
    void BasicAudioSource::DisconnectFrom(BasicAudioDestination* dest)
    {
        if (dest == NULL)
            return;
        if (dest->sources == NULL)
            return;
        int indx = dest->sources->getIndexOf(this);
        if (indx == -1)
            return;
        dest->sources->removeAt(indx);
    }
    void BasicAudioSource::Free()
    {
        if (destinations != NULL)
        {
            for (int i = 0; i < destinations->getCount(); i++)
            {
                BasicAudioDestination* dest = (BasicAudioDestination*)destinations->elementAt(i);
                if (dest != NULL)
                    DisconnectFrom(dest);
            }
            destinations->free();
            _Free(destinations);
        }
        destinations = NULL;
        if (buffer != NULL)
            buffer->Free();
        buffer = NULL;
        _Free(this);
    }

    AudioInputDevice::AudioInputDevice(const char* deviceName, AudioBuffer* buff)
    {
        this->deviceName = StrCopy(deviceName);
        this->source = new BasicAudioSource(buff);
    }

    AudioInputDevice::AudioInputDevice(const char* deviceName, BasicAudioSource* source)
    {
        this->deviceName = StrCopy(deviceName);
        this->source = source;
    }

    void AudioInputDevice::Free()
    {
        if (deviceName != NULL)
            _Free(deviceName);
        deviceName = NULL;
        if (source != NULL)
            source->Free();
        source = NULL;
        _Free(this);
    }

    AudioOutputDevice::AudioOutputDevice(const char* deviceName, AudioBuffer* buff)
    {
        this->deviceName = StrCopy(deviceName);
        this->destination = new BasicAudioDestination(buff);
    }

    AudioOutputDevice::AudioOutputDevice(const char* deviceName, BasicAudioDestination* destination)
    {
        this->deviceName = StrCopy(deviceName);
        this->destination = destination;
    }

    void AudioOutputDevice::Free()
    {
        if (deviceName != NULL)
            _Free(deviceName);
        deviceName = NULL;
        if (destination != NULL)
            destination->Free();
        destination = NULL;
        _Free(this);
    }
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