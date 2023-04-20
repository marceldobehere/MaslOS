#include "musicStruct.h"
#include "../OSDATA/MStack/MStackM.h"

namespace Music
{
    bool Note::operator== (Note other)
    {
        return note == other.note && freqLen == other.freqLen && duration == other.duration;
    }
    bool Note::operator!= (Note other)
    {
        return note != other.note || freqLen != other.freqLen || duration != other.duration;
    }

    Note::Note(int midiNote, int durationMs)
    {
        //AddToStack();
        note = midiNote;
        freqLen = getNoteDurFromMidiNote(midiNote);
        this->duration = durationMs * 1000;
        tTime = 0;
        //played = false;
        //RemoveFromStack();
    }

    // Note::Note(int durationMs)
    // {
    //     AddToStack();
    //     note = 0;
    //     freqLen = 0;
    //     this->duration = durationMs * 1000;
    //     space = true;
    //     RemoveFromStack();
    // }
    Note::Note()
    {
        //AddToStack();
        note = 1;
        freqLen = 1;
        duration = 0;
        tTime = 0;
        //played = false;
        //RemoveFromStack();
    }

    bool NoteCommand::operator== (NoteCommand other)
    {
        return duration == other.duration && isNote == other.isNote && note == other.note;
    }
    bool NoteCommand::operator!= (NoteCommand other)
    {
        return duration != other.duration || isNote != other.isNote || note != other.note;
    }

    NoteCommand::NoteCommand(int midiNote, int durationMs, bool isSingle)
    {
        //AddToStack();
        if (isSingle)
            duration = durationMs * 1000;
        else
            duration = 0;

        isNote = true;
        note = Note(midiNote, durationMs);
        //RemoveFromStack();
    }
    NoteCommand::NoteCommand(int midiNote, int durationMs)
    {
        //AddToStack();
        duration = 0;
        isNote = true;
        note = Note(midiNote, durationMs);
        //RemoveFromStack();
    }
    NoteCommand::NoteCommand(int durationMs)
    {
        //AddToStack();
        duration = durationMs * 1000;

        isNote = false;
        note = Note(0, 0);
        //RemoveFromStack();
    }
}

/*
MIDI NOTE	FREQ
97	2217.46
98	2349.32
99	2489.02
100	2637.02
101	2793.83
102	2959.96
103	3135.96
104	3322.44
105	3520.00
106	3729.31
107	3951.07
108	4186.01
109	4434.92
110	4698.64
111	4978.03
112	5274.04
113	5587.65
114	5919.91
115	6271.93
116	6644.88
117	7040.00
118	7458.62
119	7902.13
120	8372.02
121	8869.84
122	9397.27
123	9956.06
124	10548.08
125	11175.30
126	11839.82
127	12543.85

*/


namespace Music
{
    const int lookupTable[] = 
    {
        (int) (1000000 / 8.176), // 0
        (int) (1000000 / 8.662), // 1
        (int) (1000000 / 9.177), // 2
        (int) (1000000 / 9.723), // 3
        (int) (1000000 / 10.301), // 4
        (int) (1000000 / 10.913), // 5
        (int) (1000000 / 11.562), // 6
        (int) (1000000 / 12.250), // 7
        (int) (1000000 / 12.978), // 8
        (int) (1000000 / 13.750), // 9
        (int) (1000000 / 14.568), // 10
        (int) (1000000 / 15.434), // 11
        (int) (1000000 / 16.352), // 12
        (int) (1000000 / 17.324), // 13
        (int) (1000000 / 18.354), // 14
        (int) (1000000 / 19.445), // 15
        (int) (1000000 / 20.602), // 16
        (int) (1000000 / 21.827), // 17
        (int) (1000000 / 23.125), // 18
        (int) (1000000 / 24.500), // 19
        (int) (1000000 / 25.957), // 20
        (int) (1000000 / 27.500), // 21
        (int) (1000000 / 29.135), // 22
        (int) (1000000 / 30.868), // 23
        (int) (1000000 / 32.703), // 24
        (int) (1000000 / 34.648), // 25
        (int) (1000000 / 36.708), // 26
        (int) (1000000 / 38.891), // 27
        (int) (1000000 / 41.203), // 28
        (int) (1000000 / 43.654), // 29
        (int) (1000000 / 46.249), // 30
        (int) (1000000 / 48.999), // 31
        (int) (1000000 / 51.913), // 32
        (int) (1000000 / 55.000), // 33
        (int) (1000000 / 58.270), // 34
        (int) (1000000 / 61.735), // 35
        (int) (1000000 / 65.406), // 36
        (int) (1000000 / 69.296), // 37
        (int) (1000000 / 73.416), // 38
        (int) (1000000 / 77.782), // 39
        (int) (1000000 / 82.407), // 40
        (int) (1000000 / 87.307), // 41
        (int) (1000000 / 92.499), // 42
        (int) (1000000 / 97.999), // 43
        (int) (1000000 / 103.826), // 44
        (int) (1000000 / 110.000), // 45
        (int) (1000000 / 116.541), // 46
        (int) (1000000 / 123.471), // 47
        (int) (1000000 / 130.813), // 48
        (int) (1000000 / 138.591), // 49
        (int) (1000000 / 146.832), // 50
        (int) (1000000 / 155.563), // 51
        (int) (1000000 / 164.814), // 52
        (int) (1000000 / 174.614), // 53
        (int) (1000000 / 184.997), // 54
        (int) (1000000 / 195.998), // 55
        (int) (1000000 / 207.652), // 56
        (int) (1000000 / 220.000), // 57
        (int) (1000000 / 233.082), // 58
        (int) (1000000 / 246.942), // 59
        (int) (1000000 / 261.626), // 60
        (int) (1000000 / 277.183), // 61
        (int) (1000000 / 293.665), // 62
        (int) (1000000 / 311.127), // 63
        (int) (1000000 / 329.628), // 64
        (int) (1000000 / 349.228), // 65
        (int) (1000000 / 369.994), // 66
        (int) (1000000 / 391.995), // 67
        (int) (1000000 / 415.305), // 68
        (int) (1000000 / 440.000), // 69
        (int) (1000000 / 466.164), // 70
        (int) (1000000 / 493.883), // 71
        (int) (1000000 / 523.251), // 72
        (int) (1000000 / 554.365), // 73
        (int) (1000000 / 587.330), // 74
        (int) (1000000 / 622.254), // 75
        (int) (1000000 / 659.255), // 76
        (int) (1000000 / 698.456), // 77
        (int) (1000000 / 739.989), // 78
        (int) (1000000 / 783.991), // 79
        (int) (1000000 / 830.609), // 80
        (int) (1000000 / 880.000), // 81
        (int) (1000000 / 932.328), // 82
        (int) (1000000 / 987.767), // 83
        (int) (1000000 / 1046.502), // 84
        (int) (1000000 / 1108.731), // 85
        (int) (1000000 / 1174.659), // 86
        (int) (1000000 / 1244.508), // 87
        (int) (1000000 / 1318.510), // 88
        (int) (1000000 / 1396.913), // 89
        (int) (1000000 / 1479.978), // 90
        (int) (1000000 / 1567.982), // 91
        (int) (1000000 / 1661.219), // 92
        (int) (1000000 / 1760.000), // 93
        (int) (1000000 / 1864.655), // 94
        (int) (1000000 / 1975.533), // 95
        (int) (1000000 / 2093.005), // 96
        (int) (1000000 / 2217.461), // 97
        (int) (1000000 / 2349.318), // 98
        (int) (1000000 / 2489.016), // 99
        (int) (1000000 / 2637.020), // 100
        (int) (1000000 / 2793.826), // 101
        (int) (1000000 / 2959.955), // 102
        (int) (1000000 / 3135.963), // 103
        (int) (1000000 / 3322.438), // 104
        (int) (1000000 / 3520.000), // 105
        (int) (1000000 / 3729.310), // 106
        (int) (1000000 / 3951.066), // 107
        (int) (1000000 / 4186.009), // 108
        (int) (1000000 / 4434.922), // 109
        (int) (1000000 / 4698.636), // 110
        (int) (1000000 / 4978.032), // 111
        (int) (1000000 / 5274.041), // 112
        (int) (1000000 / 5587.652), // 113
        (int) (1000000 / 5919.911), // 114
        (int) (1000000 / 6271.927), // 115
        (int) (1000000 / 6644.875), // 116
        (int) (1000000 / 7040.000), // 117
        (int) (1000000 / 7458.620), // 118
        (int) (1000000 / 7902.133), // 119
        (int) (1000000 / 8372.018), // 120
        (int) (1000000 / 8869.844), // 121
        (int) (1000000 / 9397.273), // 122
        (int) (1000000 / 9956.063), // 123
        (int) (1000000 / 10548.082), // 124
        (int) (1000000 / 11175.304), // 125
        (int) (1000000 / 11839.822), // 126
        (int) (1000000 / 12543.854) // 127
    };

    int getNoteDurFromMidiNote(int midi)
    {
        if (midi >= 0 && midi <= 127)
            return lookupTable[midi];
        return 1;
    }
}




// https://www.inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies

/*
MIDI Note Number	Note Name	Pitch (Hz)
0	C-1	8.176
1	C#-1	8.662
2	D-1	9.177
3	D#-1	9.723
4	E-1	10.301
5	F-1	10.913
6	F#-1	11.562
7	G-1	12.250
8	G#-1	12.978
9	A-1	13.750
10	A#-1	14.568
11	B-1	15.434
12	C0	16.352
13	C#0	17.324
14	D0	18.354
15	D#0	19.445
16	E0	20.602
17	F0	21.827
18	F#0	23.125
19	G0	24.500
20	G#0	25.957
21	A0	27.500
22	A#0	29.135
23	B0	30.868
24	C1	32.703
25	C#1	34.648
26	D1	36.708
27	D#1	38.891
28	E1	41.203
29	F1	43.654
30	F#1	46.249
31	G1	48.999
32	G#1	51.913
33	A1	55.000
34	A#1	58.270
35	B1	61.735
36	C2	65.406
37	C#2	69.296
38	D2	73.416
39	D#2	77.782
40	E2	82.407
41	F2	87.307
42	F#2	92.499
43	G2	97.999
44	G#2	103.826
45	A2	110.000
46	A#2	116.541
47	B2	123.471
48	C3	130.813
49	C#3	138.591
50	D3	146.832
51	D#3	155.563
52	E3	164.814
53	F3	174.614
54	F#3	184.997
55	G3	195.998
56	G#3	207.652
57	A3	220.000
58	A#3	233.082
59	B3	246.942
60	C4 (Middle C)	261.626
61	C#4	277.183
62	D4	293.665
63	D#4	311.127
64	E4	329.628
65	F4	349.228
66	F#4	369.994
67	G4	391.995
68	G#4	415.305
69	A4	440.000
70	A#4	466.164
71	B4	493.883
72	C5	523.251
73	C#5	554.365
74	D5	587.330
75	D#5	622.254
76	E5	659.255
77	F5	698.456
78	F#5	739.989
79	G5	783.991
80	G#5	830.609
81	A5	880.000
82	A#5	932.328
83	B5	987.767
84	C6	1046.502
85	C#6	1108.731
86	D6	1174.659
87	D#6	1244.508
88	E6	1318.510
89	F6	1396.913
90	F#6	1479.978
91	G6	1567.982
92	G#6	1661.219
93	A6	1760.000
94	A#6	1864.655
95	B6	1975.533
96	C7	2093.005
97	C#7	2217.461
98	D7	2349.318
99	D#7	2489.016
100	E7	2637.020
101	F7	2793.826
102	F#7	2959.955
103	G7	3135.963
104	G#7	3322.438
105	A7	3520.000
106	A#7	3729.310
107	B7	3951.066
108	C8	4186.009
109	C#8	4434.922
110	D8	4698.636
111	D#8	4978.032
112	E8	5274.041
113	F8	5587.652
114	F#8	5919.911
115	G8	6271.927
116	G#8	6644.875
117	A8	7040.000
118	A#8	7458.620
119	B8	7902.133
120	C9	8372.018
121	C#9	8869.844
122	D9	9397.273
123	D#9	9956.063
124	E9	10548.080
125	F9	11175.300
126	F#9	11839.820
127	G9	12543.850
*/