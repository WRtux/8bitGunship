#pragma once

#define SAMPLE_RATE 22000

#define WAVEFORM_SINE 0
#define WAVEFORM_SQUARE 1
#define WAVEFORM_SAW 2
#define WAVEFORM_WIGGLE 4
#define WAVEFORM_STEP 6

#define OCTAVE_L 1
#define OCTAVE_2 2
#define OCTAVE_3 4
#define OCTAVE_4 8
#define OCTAVE_5 16
#define OCTAVE_6 32
#define OCTAVE_H 64
#define NOTE_C 261.63f
#define NOTEbD 277.18f
#define NOTE_D 293.66f
#define NOTEbE 311.13f
#define NOTE_E 329.63f
#define NOTE_F 349.23f
#define NOTEbG 369.99f
#define NOTE_G 392.00f
#define NOTEbA 415.30f
#define NOTE_A 440.00f
#define NOTEbB 466.16f
#define NOTE_B 493.88f

#define CALC_PITCH(oct, note) (unsigned short)((unsigned short)(oct) * (note) / 8 + 0.5f)

//#define BEAT_SPAN (SAMPLE_RATE / 2)
//#define BEATdSPAN (BEAT_SPAN * 3 / 2)
//#define BEAT_DSPAN (BEAT_SPAN * 2)
//#define BEAT_TSPAN (BEAT_SPAN * 3)
//#define BEAT_FSPAN (BEAT_SPAN * 4)
//#define BEAT_HSPAN (BEAT_SPAN / 2)
//#define BEATdHSPAN (BEAT_HSPAN * 3 / 2)
//#define BEAT_QSPAN (BEAT_SPAN / 4)
//#define BEATdQSPAN (BEAT_QSPAN * 3 / 2)
//#define BEAT_PAUSE (BEAT_SPAN / 16)
//#define BEAT_BEAT (BEAT_SPAN - BEAT_PAUSE)
//#define BEATdBEAT (BEATdSPAN - BEAT_PAUSE)
//#define BEAT_DBEAT (BEAT_DSPAN - BEAT_PAUSE)
//#define BEAT_TBEAT (BEAT_TSPAN - BEAT_PAUSE)
//#define BEAT_FBEAT (BEAT_FSPAN - BEAT_PAUSE)
//#define BEAT_HBEAT (BEAT_HSPAN - BEAT_PAUSE)
//#define BEATdHBEAT (BEATdHSPAN - BEAT_PAUSE)
//#define BEAT_QBEAT (BEAT_QSPAN - BEAT_PAUSE)
//#define BEATdQBEAT (BEATdQSPAN - BEAT_PAUSE)

#define CALC_SPAN(spd, len) (unsigned int)(SAMPLE_RATE * (unsigned short)(len) / (spd) * 60 / 240)
#define CALC_BEAT(spd, len) CALC_SPAN(spd, (len) - 10)

#define INTENSITYpPIANO 0x0BFF
#define INTENSITY_PIANO 0x13FF
#define INTENSITYpMEZZO 0x1FFF
#define INTENSITY_MEZZO 0x2BFF
#define INTENSITY_FORTE 0x3FFF
#define INTENSITYfFORTE 0x57FF
#define INTENSITY_MAX 0x7FFF

typedef struct {
	unsigned short form;
	unsigned short pitch;
	short intensity;
	unsigned short _reserved;
	unsigned int offset;
	unsigned int length;
} note_info;
typedef const char* note_simplex;

typedef FILE* file_p;
