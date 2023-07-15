#include "audioDevStuff.h"
#include "../interrupts/interrupts.h"
#include "../osData/MStack/MStackM.h"
#include "../rnd/rnd.h"
#include "../osData/osData.h"

namespace AudioDeviceStuff
{
    Audio::AudioOutputDevice* pcSpk = NULL;
    int currentRawAudioIndex = 0;
    int rawAudioDiv = 40;//60;
    bool needMoreData = true;
    bool currentState = false;

    
    void init()
    {
        currentRawAudioIndex = 0;
        needMoreData = true;
        currentState = false;
    }


    void resetTest()
    {
        osData.ac97Driver->needManualRestart = osData.ac97Driver->CheckMusic();
    }

    void checkAC97Stuff()
    {
        AddToStack();

        if (osData.ac97Driver != NULL)
        {
            int64_t currTime = (int64_t)PIT::TimeSinceBootMS();
            int64_t ac97Time = ((int64_t)(osData.ac97Driver->lastCheckTime + 500));
            if (currTime > ac97Time)
            {
                osData.ac97Driver->DoQuickCheck();
                osData.ac97Driver->HandleIRQ(NULL);
                //resetTest();
                
            }
            else if (osData.ac97Driver->doCheck)
            {
                osData.ac97Driver->doCheck = false;
                resetTest();
            }
            else if (osData.ac97Driver->needManualRestart)
            {
                resetTest();
            }
        }
        RemoveFromStack();
    }


    void play(int timeYes)
    {
        checkAC97Stuff();

        if (needMoreData && pcSpk != NULL)
        {
            currentRawAudioIndex = 0;
            int c = pcSpk->destination->RequestBuffers();
            if (c > 0)
            {
                needMoreData = false;
            }
        }

        if (needMoreData)
        {
            currentRawAudioIndex = 0;
            if (PIT::Divisor != PIT::NonMusicDiv)
                PIT::SetDivisor(PIT::NonMusicDiv);

            if (currentState)
			{
				currentState = false;
				TestSetSpeakerPosition(currentState);
			}
            return;
        }
        else
        {
            if (PIT::Divisor != rawAudioDiv)
                PIT::SetDivisor(rawAudioDiv);
        }

        AddToStack();
        {
            int indx = currentRawAudioIndex;

            uint8_t val = ((uint8_t*)pcSpk->destination->buffer->data)[indx];
            bool beep = ((((int)val)*pcSpk->destination->buffer->volume)/100) > 127;
			if (beep != currentState)
			{
				currentState = beep;
				TestSetSpeakerPosition(currentState);
			}

            currentRawAudioIndex++;
            if (currentRawAudioIndex >= pcSpk->destination->buffer->sampleCount)
            {
                needMoreData = true;
                currentRawAudioIndex = 0;
                pcSpk->destination->buffer->ClearBuffer();
                pcSpk->destination->buffer->sampleCount = pcSpk->destination->buffer->totalSampleCount;
            }
        }

        RemoveFromStack();
    }
}