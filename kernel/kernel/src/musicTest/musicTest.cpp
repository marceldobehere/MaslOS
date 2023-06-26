#include "musicTest.h"
#include "../interrupts/interrupts.h"
#include "../OSDATA/MStack/MStackM.h"
#include "../rnd/rnd.h"
#include "sbTest.h"

namespace Music
{
    List<NoteCommand>* toPlay = NULL;
    List<Note>* currentNotes  = NULL;
    List<uint8_t>* currentRawAudio = NULL;
    bool rawAudioInUse = false;
    int currentRawAudioIndex = 0;
    int rawAudioDiv = 60;
    int currentIndex = 0;
	int currentCount = -1;

    bool listInUse = false;
    //int64_t tempTime = 0; // MICRO S
    bool currentState = false;
    bool paused;
    int volume = 50; 
    
    void init()
    {
        toPlay = new List<NoteCommand>(10);
        currentNotes = new List<Note>(10);
        currentRawAudio = new List<uint8_t>(10);
        currentRawAudioIndex = 0;
        rawAudioInUse = false;
        currentIndex = 0;
		currentCount = 0;
        listInUse = false;
        paused = false;
    }


    void addCmd(NoteCommand note)
    {
        if (toPlay == NULL)
            return;

        //AddToStack();
        listInUse = true;
        toPlay->add(note);
        listInUse = false;
        //RemoveFromStack();
    }

    

    void play2()
    {
        AddToStack();
        // if (osData.ac97Driver != NULL && (PIT::TicksSinceBoot % 44) == 0)
        // {
        //     //AddToStack();
        //     int amt = 8000;
        //     int step = 1;
        //     int hz = ((PIT::TimeSinceBootMS() / 10) % 100) * 4 + 200;
        //     //hz = 400;
        //     uint16_t* testBuff = (uint16_t*)_Malloc(amt*2);

        //     MusicBit16Test::FillArray(testBuff, 0, amt, hz);            
        //     // for (int i = 0; i + step <= amt; i += step)
        //     // {
        //     //     //AddToStack();
        //     //     MusicBit16Test::FillArray(testBuff, i, step, hz);
        //     //     //Println(window, "HZ: {}", to_string(hz));
        //     //     //hz += 10;
        //     //     //RemoveFromStack();
        //     // }
        //     //RemoveFromStack();
            
        //     //AddToStack();
        //     uint64_t tCount = 0;
        //     tCount = osData.ac97Driver->writeBuffer(0, (uint8_t*)testBuff, amt*2);
        //     //RemoveFromStack();


        //     //AddToStack();
        //     _Free(testBuff);
        //     //RemoveFromStack();
        // }
        RemoveFromStack();
    }


    void play(int timeYes)
    {
        play2();

        if (toPlay == NULL || paused)
            return;


        AddToStack();
        if (!rawAudioInUse && currentRawAudio->getCount() > 0)
        {
            if (PIT::Divisor != rawAudioDiv)
                PIT::SetDivisor(rawAudioDiv);

            int indx1 = currentRawAudioIndex >> 3;
            int indx2 = currentRawAudioIndex & 0b111;

            uint8_t val = currentRawAudio->elementAt(indx1);
            uint8_t yes = (val >> (7 - indx2)) & 1;
            bool beep = yes == 1;
			if (beep != currentState)
			{
				currentState = beep;
				TestSetSpeakerPosition(currentState);
			}

            currentRawAudioIndex++;
            if (currentRawAudioIndex >= 20000)
            {
                currentRawAudioIndex = 0;
                currentRawAudio->removeFirst(20000 / 8);
            }
            if (currentRawAudioIndex >> 3 >= currentRawAudio->getCount())
            {
                currentRawAudio->clear();
                currentRawAudioIndex = 0;
            }

            RemoveFromStack();
            return;
        }
        currentRawAudioIndex = 0;

        
        // Add Note if needed
        testo:
        if (!listInUse && toPlay->getCount() > 0) 
        {
            NoteCommand n = toPlay->elementAt(0);
            n.duration -= timeYes;
            if (n.isNote)
            {
                n.isNote = false;
                currentCount = -1;
                currentNotes->add(n.note);
            }

            if (n.duration <= 0)
            {
                toPlay->removeAt(0);
                goto testo;
            }
            else
                toPlay->set(0, n);
        }

        // Set Div if needed
        if (currentNotes->getCount() > 0 || toPlay->getCount() > 0)
        {
            if (PIT::Divisor != PIT::MusicDiv)
                PIT::SetDivisor(PIT::MusicDiv);
        }
        else
        {
            if (PIT::Divisor != PIT::NonMusicDiv)
                PIT::SetDivisor(PIT::NonMusicDiv);
        }

        // play current note
        if (currentNotes->getCount() > 0)
        {
            //if (currentIndex >= currentNotes->getCount())
            //    currentIndex = currentIndex % currentNotes->getCount();
            
            //Note t = currentNotes->elementAt(currentIndex);
			
            // if (t.duration > 0)
            // {
            //     t.tTime += timeYes;
            //     bool inc = false;
            //     if (t.tTime * 2 >= t.freqLen)
            //     {
            //         currentState = !currentState;
            //         TestSetSpeakerPosition(currentState);
            //         t.tTime -= t.freqLen / 2;
            //         inc = true;
            //     }
            //     currentNotes->set(currentIndex, t);
            //     if (inc)
            //         currentIndex++;
            // }
			
            // Subtract Times and Remove if needed
			bool playBeep = false;
			currentIndex %= currentNotes->getCount();
			if (currentCount == -1)
            {
                int a = currentNotes->elementAt(currentIndex).freqLen;
                int last = currentNotes->elementAt(currentNotes->getCount() - 1).freqLen;
                if (a == 0)
                    a = 1;
                if (last == 0)
                    last == 1;
                currentCount = (last + last / 2) / a;
            }
			//currentCount = (currentNotes->getCount() - currentIndex) * 2;
            for (int i = 0; i < currentNotes->getCount(); i++)
            {
                Note t = currentNotes->elementAt(i);
				t.duration -= timeYes;

                //if (currentIndex == i)
				t.tTime += timeYes * 2;
				
				if (t.tTime >= t.freqLen &&
                    t.tTime <= t.freqLen + (t.freqLen * volume) / 100)
                {
                    //t.tTime -= t.freqLen;
					if (currentIndex == i)
                    {
                        playBeep = true;

                    }
                }
				if (t.tTime >= t.freqLen * 2)
                {
                    t.tTime -= t.freqLen  * 2;
					//playBeep = false;
					if (currentIndex == i && t.duration > 0)
						currentCount--;
					if (currentCount < 1)
					{
						currentIndex++;
                        currentIndex %= currentNotes->getCount();

						currentCount = -1;
                        {
                            int a = currentNotes->elementAt(currentIndex).freqLen;
                            int last = currentNotes->elementAt(currentNotes->getCount() - 1).freqLen;
                            if (a == 0)
                                a = 1;
                            if (last == 0)
                                last == 1;
                            currentCount = last / a;
                        }
					}
                }
				
                if (t.duration > 0)
                    currentNotes->set(i, t);
                else
                {
                    currentNotes->removeAt(i);
                    i--;
					currentCount = -1;
                }
            }
			//currentIndex++;

			// if (playBeep)
			// {
			// 	currentState = !currentState;
			// 	TestSetSpeakerPosition(currentState);
			// }
			if (playBeep != currentState)
			{
				currentState = playBeep;
				TestSetSpeakerPosition(currentState);
			}
        }
        else
        {
            if (currentState)
            {
                TestSetSpeakerPosition(false);
                currentState = false;
				currentIndex = 0;
				currentCount = -1;
            }
        }

        RemoveFromStack();

        // AddToStack();
        // if (toPlay->getCount() < 1)
        // {
        //     if (currentState)
        //     {
        //         TestSetSpeakerPosition(false);
        //         currentState = false;
        //         tempTime = 0;
        //     }
        //     if (PIT::Divisor == PIT::MusicDiv)
        //     {
        //         PIT::SetDivisor(PIT::NonMusicDiv);
        //     }
        //     RemoveFromStack();
        //     return;
        // }
        // if (PIT::Divisor == PIT::NonMusicDiv)
        // {
        //     PIT::SetDivisor(PIT::MusicDiv);
        // }

        // Note t = toPlay->elementAt(0);
        // if (t.duration > 0)
        // {
        //     tempTime += timeYes;
        //     if (!t.space)
        //     {
        //         if (tempTime * 2 >= t.freqLen)
        //         {
        //             currentState = !currentState;
        //             TestSetSpeakerPosition(currentState);
        //             tempTime -= t.freqLen / 2;
        //         }
        //     }
        //     else
        //     {
        //         if (currentState)
        //         {
        //             TestSetSpeakerPosition(false);
        //             currentState = false;
        //         }
        //     }

            
        //     t.duration -= timeYes;
        //     toPlay->set(0, t);
        // }
        // else
        // {
        //     if (listInUse)
        //     {
        //         RemoveFromStack();
        //         return;
        //     }
        //     toPlay->removeFirst();
        //     tempTime = 0;
        // }
        // 
        // RemoveFromStack();
    }







    void addMario()
    {
        AddToStack();
        paused = true;

        toPlay->clear();
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(296));
        addCmd(NoteCommand(60, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(78, 74));
        addCmd(NoteCommand(75, 74));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(78, 74));
        addCmd(NoteCommand(75, 74));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(78, 74));
        addCmd(NoteCommand(75, 74));
        addCmd(NoteCommand(296));
        addCmd(NoteCommand(60, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(88, 74));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(77, 74));
        addCmd(NoteCommand(62, 74));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(88, 75));
        addCmd(NoteCommand(84, 75));
        addCmd(NoteCommand(77, 75));
        addCmd(NoteCommand(62, 75));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(88, 74));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(77, 74));
        addCmd(NoteCommand(62, 74));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(81, 74));
        addCmd(NoteCommand(77, 74));
        addCmd(NoteCommand(62, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(88, 74));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(77, 74));
        addCmd(NoteCommand(62, 74));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(91, 74));
        addCmd(NoteCommand(86, 74));
        addCmd(NoteCommand(83, 74));
        addCmd(NoteCommand(67, 74));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(59, 74));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(48, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(64, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(63, 75));
        addCmd(NoteCommand(55, 75));
        addCmd(NoteCommand(52, 75));
        addCmd(NoteCommand(75));
        addCmd(NoteCommand(64, 148));
        addCmd(NoteCommand(43, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(43, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(67, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(48, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(48, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(67, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(43, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(43, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(54, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(74, 74));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(54, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(54, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(71, 74));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 149));
        addCmd(NoteCommand(54, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(72, 222));
        addCmd(NoteCommand(65, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(53, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 75));
        addCmd(NoteCommand(65, 75));
        addCmd(NoteCommand(57, 75));
        addCmd(NoteCommand(53, 75));
        addCmd(NoteCommand(75));
        addCmd(NoteCommand(50, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(71, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(71, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(67, 223));
        addCmd(NoteCommand(64, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(76, 148));
        addCmd(NoteCommand(63, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(63, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(79, 111));
        addCmd(NoteCommand(63, 111));
        addCmd(NoteCommand(111));
        addCmd(NoteCommand(79, 37));
        addCmd(NoteCommand(62, 37));
        addCmd(NoteCommand(37));
        addCmd(NoteCommand(77, 185));
        addCmd(NoteCommand(62, 185));
        addCmd(NoteCommand(185));
        addCmd(NoteCommand(77, 37));
        addCmd(NoteCommand(62, 37));
        addCmd(NoteCommand(37));
        addCmd(NoteCommand(79, 186));
        addCmd(NoteCommand(62, 186));
        addCmd(NoteCommand(186));
        addCmd(NoteCommand(79, 37));
        addCmd(NoteCommand(62, 37));
        addCmd(NoteCommand(37));
        addCmd(NoteCommand(62, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(79, 111));
        addCmd(NoteCommand(53, 111));
        addCmd(NoteCommand(111));
        addCmd(NoteCommand(79, 37));
        addCmd(NoteCommand(55, 37));
        addCmd(NoteCommand(37));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(48, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(64, 149));
        addCmd(NoteCommand(55, 149));
        addCmd(NoteCommand(52, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(63, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 148));
        addCmd(NoteCommand(43, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(43, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(67, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(48, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(48, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(67, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 149));
        addCmd(NoteCommand(52, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(43, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(43, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(54, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(74, 74));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(54, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(54, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(50, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(68, 75));
        addCmd(NoteCommand(50, 75));
        addCmd(NoteCommand(75));
        addCmd(NoteCommand(69, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(54, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(50, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(75, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 149));
        addCmd(NoteCommand(53, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(76, 222));
        addCmd(NoteCommand(71, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(75, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(67, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 223));
        addCmd(NoteCommand(57, 223));
        addCmd(NoteCommand(53, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(72, 222));
        addCmd(NoteCommand(68, 222));
        addCmd(NoteCommand(51, 222));
        addCmd(NoteCommand(48, 222));
        addCmd(NoteCommand(370));
        addCmd(NoteCommand(72, 222));
        addCmd(NoteCommand(65, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(46, 222));
        addCmd(NoteCommand(296));
        addCmd(NoteCommand(72, 222));
        addCmd(NoteCommand(64, 222));
        addCmd(NoteCommand(48, 222));
        addCmd(NoteCommand(43, 222));
        addCmd(NoteCommand(889));
        addCmd(NoteCommand(60, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(62, 223));
        addCmd(NoteCommand(50, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(62, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(53, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(62, 222));
        addCmd(NoteCommand(45, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(62, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(53, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(50, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(57, 149));
        addCmd(NoteCommand(53, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(64, 222));
        addCmd(NoteCommand(60, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(62, 148));
        addCmd(NoteCommand(60, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(64, 75));
        addCmd(NoteCommand(60, 75));
        addCmd(NoteCommand(57, 75));
        addCmd(NoteCommand(53, 75));
        addCmd(NoteCommand(75));
        addCmd(NoteCommand(60, 148));
        addCmd(NoteCommand(45, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(60, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(67, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(60, 148));
        addCmd(NoteCommand(50, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(76, 148));
        addCmd(NoteCommand(69, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(77, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(78, 149));
        addCmd(NoteCommand(69, 149));
        addCmd(NoteCommand(45, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(76, 148));
        addCmd(NoteCommand(69, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(71, 222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(67, 222));
        addCmd(NoteCommand(52, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(71, 148));
        addCmd(NoteCommand(69, 148));
        addCmd(NoteCommand(67, 148));
        addCmd(NoteCommand(59, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(70, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(59, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(71, 223));
        addCmd(NoteCommand(69, 223));
        addCmd(NoteCommand(67, 223));
        addCmd(NoteCommand(52, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(71, 148));
        addCmd(NoteCommand(69, 148));
        addCmd(NoteCommand(67, 148));
        addCmd(NoteCommand(59, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(70, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(59, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(71, 222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(67, 222));
        addCmd(NoteCommand(52, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(71, 148));
        addCmd(NoteCommand(69, 148));
        addCmd(NoteCommand(67, 148));
        addCmd(NoteCommand(64, 148));
        addCmd(NoteCommand(62, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(70, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(62, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(71, 148));
        addCmd(NoteCommand(69, 148));
        addCmd(NoteCommand(67, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(74, 149));
        addCmd(NoteCommand(71, 149));
        addCmd(NoteCommand(64, 149));
        addCmd(NoteCommand(62, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(62, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(71, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(71, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 75));
        addCmd(NoteCommand(61, 75));
        addCmd(NoteCommand(75));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(68, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 148));
        addCmd(NoteCommand(64, 148));
        addCmd(NoteCommand(61, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(67, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(53, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(65, 148));
        addCmd(NoteCommand(60, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(65, 148));
        addCmd(NoteCommand(48, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(48, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(72, 148));
        addCmd(NoteCommand(60, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(75, 74));
        addCmd(NoteCommand(68, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(75, 223));
        addCmd(NoteCommand(68, 223));
        addCmd(NoteCommand(53, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(74, 222));
        addCmd(NoteCommand(68, 222));
        addCmd(NoteCommand(60, 222));
        addCmd(NoteCommand(56, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(72, 222));
        addCmd(NoteCommand(68, 222));
        addCmd(NoteCommand(48, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(74, 222));
        addCmd(NoteCommand(68, 222));
        addCmd(NoteCommand(60, 222));
        addCmd(NoteCommand(56, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(71, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 149));
        addCmd(NoteCommand(71, 149));
        addCmd(NoteCommand(62, 149));
        addCmd(NoteCommand(59, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(75, 74));
        addCmd(NoteCommand(70, 74));
        addCmd(NoteCommand(62, 74));
        addCmd(NoteCommand(59, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(76, 148));
        addCmd(NoteCommand(71, 148));
        addCmd(NoteCommand(50, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(78, 74));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(79, 148));
        addCmd(NoteCommand(74, 148));
        addCmd(NoteCommand(62, 148));
        addCmd(NoteCommand(59, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(83, 74));
        addCmd(NoteCommand(74, 74));
        addCmd(NoteCommand(62, 74));
        addCmd(NoteCommand(59, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(83, 74));
        addCmd(NoteCommand(74, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(82, 74));
        addCmd(NoteCommand(74, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 149));
        addCmd(NoteCommand(61, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(81, 222));
        addCmd(NoteCommand(73, 222));
        addCmd(NoteCommand(52, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(64, 222));
        addCmd(NoteCommand(61, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(53, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(83, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(50, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(83, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(60, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(86, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(83, 74));
        addCmd(NoteCommand(59, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(59, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(59, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(74, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(48, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(48, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(75, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(48, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(75, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(518));
        addCmd(NoteCommand(43, 445));
        addCmd(NoteCommand(445));
        addCmd(NoteCommand(48, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(64, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(63, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 148));
        addCmd(NoteCommand(43, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(43, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(67, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(48, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(48, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(67, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(43, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(43, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 223));
        addCmd(NoteCommand(69, 223));
        addCmd(NoteCommand(50, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(74, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(54, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(76, 222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(76, 223));
        addCmd(NoteCommand(69, 223));
        addCmd(NoteCommand(57, 223));
        addCmd(NoteCommand(54, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(45, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(54, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(59, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(59, 149));
        addCmd(NoteCommand(55, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(50, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(75, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(59, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(59, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 222));
        addCmd(NoteCommand(71, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(75, 148));
        addCmd(NoteCommand(63, 148));
        addCmd(NoteCommand(59, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(59, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(50, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(59, 222));
        addCmd(NoteCommand(55, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(68, 74));
        addCmd(NoteCommand(63, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(370));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(62, 74));
        addCmd(NoteCommand(58, 74));
        addCmd(NoteCommand(297));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(1111));
        addCmd(NoteCommand(87, 74));
        addCmd(NoteCommand(83, 74));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(88, 74));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(87, 74));
        addCmd(NoteCommand(83, 74));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(88, 74));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(84, 74));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(83, 74));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(84, 75));
        addCmd(NoteCommand(75));
        addCmd(NoteCommand(83, 74));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(82, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(81, 74));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(80, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(444));
        addCmd(NoteCommand(48, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(64, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(63, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 148));
        addCmd(NoteCommand(43, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(43, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(67, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(48, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 75));
        addCmd(NoteCommand(48, 75));
        addCmd(NoteCommand(75));
        addCmd(NoteCommand(67, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(43, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(43, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(57, 149));
        addCmd(NoteCommand(54, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(74, 74));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(54, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 223));
        addCmd(NoteCommand(66, 223));
        addCmd(NoteCommand(57, 223));
        addCmd(NoteCommand(54, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(71, 74));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(54, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(72, 222));
        addCmd(NoteCommand(65, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(53, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 75));
        addCmd(NoteCommand(75));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(50, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(71, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(71, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(67, 222));
        addCmd(NoteCommand(64, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(76, 148));
        addCmd(NoteCommand(63, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(63, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(79, 112));
        addCmd(NoteCommand(63, 112));
        addCmd(NoteCommand(112));
        addCmd(NoteCommand(79, 37));
        addCmd(NoteCommand(62, 37));
        addCmd(NoteCommand(37));
        addCmd(NoteCommand(77, 185));
        addCmd(NoteCommand(62, 185));
        addCmd(NoteCommand(185));
        addCmd(NoteCommand(77, 37));
        addCmd(NoteCommand(62, 37));
        addCmd(NoteCommand(37));
        addCmd(NoteCommand(79, 185));
        addCmd(NoteCommand(62, 185));
        addCmd(NoteCommand(185));
        addCmd(NoteCommand(79, 37));
        addCmd(NoteCommand(62, 37));
        addCmd(NoteCommand(37));
        addCmd(NoteCommand(62, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(79, 111));
        addCmd(NoteCommand(53, 111));
        addCmd(NoteCommand(111));
        addCmd(NoteCommand(79, 37));
        addCmd(NoteCommand(55, 37));
        addCmd(NoteCommand(37));
        addCmd(NoteCommand(55, 75));
        addCmd(NoteCommand(75));
        addCmd(NoteCommand(48, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(64, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(63, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(64, 148));
        addCmd(NoteCommand(43, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(43, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(67, 148));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(48, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(48, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(67, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(43, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(43, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(55, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(55, 148));
        addCmd(NoteCommand(52, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 223));
        addCmd(NoteCommand(66, 223));
        addCmd(NoteCommand(50, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(54, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(74, 74));
        addCmd(NoteCommand(66, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(54, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(66, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 223));
        addCmd(NoteCommand(66, 223));
        addCmd(NoteCommand(57, 223));
        addCmd(NoteCommand(54, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(50, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(68, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(54, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(54, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(50, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(79, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 149));
        addCmd(NoteCommand(53, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(75, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 222));
        addCmd(NoteCommand(71, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(75, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(52, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(67, 74));
        addCmd(NoteCommand(52, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(69, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(53, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(72, 222));
        addCmd(NoteCommand(68, 222));
        addCmd(NoteCommand(51, 222));
        addCmd(NoteCommand(48, 222));
        addCmd(NoteCommand(370));
        addCmd(NoteCommand(72, 222));
        addCmd(NoteCommand(65, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(46, 222));
        addCmd(NoteCommand(297));
        addCmd(NoteCommand(72, 222));
        addCmd(NoteCommand(64, 222));
        addCmd(NoteCommand(48, 222));
        addCmd(NoteCommand(43, 222));
        addCmd(NoteCommand(888));
        addCmd(NoteCommand(60, 149));
        addCmd(NoteCommand(149));
        addCmd(NoteCommand(61, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(62, 222));
        addCmd(NoteCommand(50, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(62, 222));
        addCmd(NoteCommand(57, 222));
        addCmd(NoteCommand(53, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(62, 222));
        addCmd(NoteCommand(45, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(62, 223));
        addCmd(NoteCommand(57, 223));
        addCmd(NoteCommand(53, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(76, 74));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(50, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(50, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(72, 74));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(45, 222));
        addCmd(NoteCommand(222));
        addCmd(NoteCommand(69, 74));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(64, 223));
        addCmd(NoteCommand(60, 223));
        addCmd(NoteCommand(50, 223));
        addCmd(NoteCommand(223));
        addCmd(NoteCommand(62, 148));
        addCmd(NoteCommand(60, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(64, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(60, 148));
        addCmd(NoteCommand(45, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(65, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(45, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(60, 148));
        addCmd(NoteCommand(57, 148));
        addCmd(NoteCommand(53, 148));
        addCmd(NoteCommand(148));
        addCmd(NoteCommand(67, 74));
        addCmd(NoteCommand(60, 74));
        addCmd(NoteCommand(57, 74));
        addCmd(NoteCommand(53, 74));
        addCmd(NoteCommand(74));
        addCmd(NoteCommand(60, 148));

    
        paused = false;
        RemoveFromStack();
    }

}