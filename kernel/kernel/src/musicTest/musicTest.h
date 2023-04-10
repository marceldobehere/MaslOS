#pragma once
#include "musicStruct.h"
#include "../customClasses/list.h"


namespace Music
{
    extern List<NoteCommand>* toPlay;
    extern List<Note>* currentNotes;
    extern int currentIndex;
    void init();
    extern bool listInUse;
    void addCmd(NoteCommand note);
    void play(int timeYes);
    extern bool paused;

    int getNoteDurFromMidiNote(int midi);

    void addRick();

}