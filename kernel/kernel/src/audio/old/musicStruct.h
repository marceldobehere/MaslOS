// #pragma once
// #include "stdint.h"


// namespace Music
// {
//     struct Note
//     {
//         uint8_t note; // MIDI NOTE
//         int32_t freqLen;
//         int32_t duration; // MICRO S
//         int32_t tTime;
//         //bool played;
//         //bool space;

//         bool operator== (Note other);
//         bool operator!= (Note other);

//         Note(int midiNote, int durationMs);
//         Note();
        
//     };

//     struct NoteCommand
//     {
//         int duration;
//         bool isNote;
//         Note note;

//         bool operator== (NoteCommand other);
//         bool operator!= (NoteCommand other);

//         NoteCommand(int midiNote, int durationMs, bool isSingle);
//         NoteCommand(int midiNote, int durationMs);
//         NoteCommand(int durationMs);
//     };

//     extern const int lookupTable[];
//     int getNoteDurFromMidiNote(int midi);
// }

