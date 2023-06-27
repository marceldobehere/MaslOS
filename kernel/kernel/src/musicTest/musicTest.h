#pragma once
#include "musicStruct.h"
#include "../customClasses/list.h"


namespace Music
{
    extern List<NoteCommand>* toPlay;
    extern List<Note>* currentNotes;
    extern List<uint8_t>* currentRawAudio;
    extern bool rawAudioInUse;
    extern int currentRawAudioIndex;
    extern int currentIndex;
    extern int currentCount;
    extern int rawAudioDiv; // 59.6591 
    void init();
    extern bool listInUse;
    void addCmd(NoteCommand note);
    void play(int timeYes);
    extern bool paused;
    extern int volume;
    void resetTest();

    int getNoteDurFromMidiNote(int midi);

    void addMario();

}