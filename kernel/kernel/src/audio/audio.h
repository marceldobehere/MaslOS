#pragma once
#include "../customClasses/list.h"

namespace Audio
{

    class AudioBuffer
    {
        public:
        int bitsPerSample;
        int sampleRate;
        int channelCount; 
        int totalSampleCount; // ignoring channelCount
        int sampleCount; // ignoring channelCount
        int volume;
        long byteCount;

        void* data;

        AudioBuffer(int bitsPerSample, int sampleRate, int channelCount, int sampleCount);
        int MixBuffer(AudioBuffer* other, int sampleOffset);
        void ClearBuffer();
        void Free();

        static AudioBuffer* Create16BitMonoBuffer(int sampleRate, int sampleCount);
        static AudioBuffer* Create16Bit48KHzMonoBuffer(int sampleCount);
        static AudioBuffer* Create16BitStereoBuffer(int sampleRate, int sampleCount);
        static AudioBuffer* Create16Bit48KHzStereoBuffer(int sampleCount);
    };

    class BasicAudioSource;
    class BasicAudioDestination;

    class BasicAudioDestination
    {
        public:
        List<void*>* sources; // BasicAudioSource*
        AudioBuffer* buffer;

        BasicAudioDestination(AudioBuffer* buffer);
        int RequestBuffer(BasicAudioSource* from);
        int RequestBuffers();
        bool AllBuffersDone();
        void Free();
    };

    class BasicAudioSource
    {
        public:
        List<void*>* destinations;
        AudioBuffer* buffer;
        int samplesSent;
        bool readyToSend;
        void* OnFinishHelp;
        void (*OnFinish)(void* bruh, BasicAudioDestination* dest);

        BasicAudioSource(AudioBuffer* buffer);
        void ConnectTo(BasicAudioDestination* dest);
        void DisconnectFrom(BasicAudioDestination* dest);
        void Free();
    };

}