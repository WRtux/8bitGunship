#include <stdio.h>
#include "Helper.h"

#define BEAT_SPEED 120
#define _SPAN(len) CALC_SPAN(BEAT_SPEED, len)
#define _BEAT(len) CALC_BEAT(BEAT_SPEED, len)

const note_simplex notes[] = {
	"W4D.m", "W4D.m", "W5D.m", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W4C.m", "W4C.m", "W5D.m", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W3B.m", "W3B.m", "W5D.m", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W3b.m", "W3b.m", "W5D.m", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W4D.m", "W4D.m", "W5D.m", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W4C.m", "W4C.m", "W5D.m", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W3B.m", "W3B.m", "W5D.m", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W3b.m", "W3b.m", "W5D.m", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W4D6D. M", "W4D6D. M", "W5D6D. M", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W4C6D. M", "W4C6D. M", "W5D6D. M", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W3B6D. M", "W3B6D. M", "W5D6D. M", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W3b6D. M", "W3b6D. M", "W5D6D. M", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W4D6D. M", "W4D6D. M", "W5D6D. M", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W4C6D. M", "W4C6D. M", "W5D6D. M", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W3B6D. M", "W3B6D. M", "W5D6D. M", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"W3b6D. M", "W3b6D. M", "W5D6D. M", "R.", "W4A.m", "R,", "W4a.m", "R.", "W4G.m", "R.", "W4F,m", "W4D.m", "W4F.m", "W4G.m",
	"-4", "-4", "-4", "-4",
	"<0",
	"-4", "-4", "-4", "-4",
	"-4", "-4", "-4", "-4",
	"S5D.m", "S5D.m", "S6D.m", "R.", "S5A.m", "R,", "S5a.m", "R.", "S5G.m", "R.", "S5F,m", "S5D.m", "S5F.m", "S5G.m",
	"S5C.m", "S5C.m", "S6D.m", "R.", "S5A.m", "R,", "S5a.m", "R.", "S5G.m", "R.", "S5F,m", "S5D.m", "S5F.m", "S5G.m",
	"S4B.m", "S4B.m", "S6D.m", "R.", "S5A.m", "R,", "S5a.m", "R.", "S5G.m", "R.", "S5F,m", "S5D.m", "S5F.m", "S5G.m",
	"S4b.m", "S4b.m", "S6D.m", "R.", "S5A.m", "R,", "S5a.m", "R.", "S5G.m", "R.", "S5F,m", "S5D.m", "S5F.m", "S5G.m",
	"S5D.m", "S5D.m", "S6D.m", "R.", "S5A.m", "R,", "S5a.m", "R.", "S5G.m", "R.", "S5F,m", "S5D.m", "S5F.m", "S5G.m",
	"S5C.m", "S5C.m", "S6D.m", "R.", "S5A.m", "R,", "S5a.m", "R.", "S5G.m", "R.", "S5F,m", "S5D.m", "S5F.m", "S5G.m",
	"S4B.m", "S4B.m", "S6D.m", "R.", "S5A.m", "R,", "S5a.m", "R.", "S5G.m", "R.", "S5F,m", "S5D.m", "S5F.m", "S5G.m",
	"S4b.m", "S4b.m", "S6D.m", "R.", "S5A.m", "R,", "S5a.m", "R.", "S5G.m", "R.", "S5F,m", "S5D.m", "S5F.m", "S5G.m",
	"Q5F,M", "Q5F.M", "Q5F.M", "R.", "Q5F.M", "R.", "Q5F,M", "Q5D.M", "R.", "Q5D1M", "R.",
	"Q5F,M", "Q5F.M", "Q5F.M", "R.", "Q5G.M", "R.", "Q5a,M", "Q5G5a5G.,f", "Q5F.M", "Q5D.M", "Q5F.M", "Q5G.M", "R,",
	"Q5F,M", "Q5F.M", "Q5F.M", "R.", "Q5G.M", "R.", "Q5a.M", "R.", "Q5A.M", "R.", "Q6C.M", "R.", "Q5A;M",
	"Q6D.M", "R.", "Q6D.M", "R.", "Q6D.M", "Q5A.M", "Q6D.M", "Q6C6d6D2>M", "R.",
	"<0",
	"-4", "-4", "-4", "-4",
	"W2D,m", "W2D,m", "W2D.m", "W2D,m", "W2D,m", "W2D,m", "W2D.m", "W2D.m", "W2D.m", "W2D,m",
	"W2C,m", "W2C,m", "W2C.m", "W2C,m", "W2C,m", "W2C,m", "W2C.m", "W2C.m", "W2C.m", "W2C,m",
	"W1B,m", "W1B,m", "W1B.m", "W1B,m", "W1B,m", "W1B,m", "W1B.m", "W1B.m", "W1B.m", "W1B,m",
	"W1b,m", "W1b,m", "W1b.m", "W1b,m", "W2C,m", "W2C,m", "W2C.m", "W2C.m", "W2C.m", "W2C,m",
	"W2D3D, M", "W2D3D, M", "W2D3D. M", "W2D3D, M", "W2D3D, M", "W2D3D, M", "W2D3D. M", "W2D3D. M", "W2D3D. M", "W2D3D, M",
	"W2C3C, M", "W2C3C, M", "W2C3C. M", "W2C3C, M", "W2C3C, M", "W2C3C, M", "W2C3C. M", "W2C3C. M", "W2C3C. M", "W2C3C, M",
	"W1B2B, M", "W1B2B, M", "W1B2B. M", "W1B2B, M", "W1B2B, M", "W1B2B, M", "W1B2B. M", "W1B2B. M", "W1B2B. M", "W1B2B, M",
	"W1b2b, M", "W1b2b, M", "W1b2b. M", "W1b2b, M", "W2C3C, M", "W2C3C, M", "W2C3C. M", "W2C3C. M", "W2C3C. M", "W2C3C, M",
	"W2D3D, M", "W2D3D, M", "W2D3D. M", "W2D3D, M", "W2D3D, M", "W2D3D, M", "W2D3D. M", "W2D3D. M", "W2D3D. M", "W2D3D, M",
	"W2C3C, M", "W2C3C, M", "W2C3C. M", "W2C3C, M", "W2C3C, M", "W2C3C, M", "W2C3C. M", "W2C3C. M", "W2C3C. M", "W2C3C, M",
	"W1B2B, M", "W1B2B, M", "W1B2B. M", "W1B2B, M", "W1B2B, M", "W1B2B, M", "W1B2B. M", "W1B2B. M", "W1B2B. M", "W1B2B, M",
	"W1b2b, M", "W1b2b, M", "W1b2b. M", "W1b2b, M", "W2C3C, M", "W2C3C, M", "W2C3C. M", "W2C3C. M", "W2C3C. M", "W2C3C, M",
	"W2D3D, M", "W2D3D, M", "W2D3D. M", "W2D3D, M", "W2D3D, M", "W2D3D, M", "W2D3D. M", "W2D3D. M", "W2D3D. M", "W2D3D, M",
	"W2C3C, M", "W2C3C, M", "W2C3C. M", "W2C3C, M", "W2C3C, M", "W2C3C, M", "W2C3C. M", "W2C3C. M", "W2C3C. M", "W2C3C, M",
	"W2D3D, M", "W2D3D, M", "W2D3D. M", "W2D3D, M", "W2D3D, M", "W2D3D, M", "W2D3D. M", "W2D3D. M", "W2D3D. M", "W2D3D, M",
	"W2C3C, M", "W2C3C, M", "W2C3C. M", "W2C3C, M", "W2C3C, M", "W2C3C, M", "W2C3C. M", "W2C3C. M", "W2C3C. M", "W2C3C, M",
	"<0",
	"-4", "-4", "-4", "-4",
	"-4", "-4", "-4", "-4",
	"S3D3A4 P", "S3C3G4 P", "S2B3g4 P", "S2b3F2 P", "S3C3G2 P",
	"S3D3A4 P", "S3C3G4 P", "S2B3g4 P", "S2b3F2 P", "S3C3G2 P",
	"-4", "-4", "-4", "-4",
	"<0",
	"-4", "-4", "-4", "-4",
	"-4", "-4", "-4", "-4",
	"s1F3F, f", "s2C3F, M", "s1F3F, M", "s2C3F. M", "s1F.m", "s3F.m", "s1F.m", "s2C3F, M", "s1F3F. M", "s1F.m", "s2C3F. M", "s1F.m",
	"s1F3F, M", "s2C3F, M", "s1F3F, M", "s2C3F. M", "s1F.m", "s3F.m", "s1F.m", "s2C3F, M", "s1F3F. M", "s1F.m", "s2C3F. M", "s1F.m",
	"s1F3F, M", "s2C3F, M", "s1F3F, M", "s2C3F. M", "s1F.m", "s3F.m", "s1F.m", "s2C3F, M", "s1F3F. M", "s1F.m", "s2C3F. M", "s1F.m",
	"s1F3F, M", "s2C3F, M", "s1F3F, M", "s2C3F. M", "s1F.m", "s2C3F. f", "s1F.m", "s1F2C3F. f", "s2C.M", "s1F3F. M", "s1F2C. M", "s2C3F. f", "s1F1F.;m",
	"s1F3F, f", "s2C3F, M", "s1F3F, M", "s2C3F. M", "s1F.m", "s3F.m", "s1F.m", "s2C3F, M", "s1F3F. M", "s1F.m", "s2C3F. M", "s1F.m",
	"s1F3F, M", "s2C3F, M", "s1F3F, M", "s2C3F. M", "s1F.m", "s3F.m", "s1F.m", "s2C3F, M", "s1F3F. M", "s1F.m", "s2C3F. M", "s1F.m",
	"s1F3F, M", "s2C3F, M", "s1F3F, M", "s2C3F. M", "s1F.m", "s3F.m", "s1F.m", "s2C3F, M", "s1F3F. M", "s1F.m", "s2C3F. M", "s1F.m",
	"s1F3F, M", "s2C3F, M", "s1F3F, M", "s2C3F. M", "s1F.m", "s2C3F. f", "s1F.m", "s1F2C3F. f", "s2C.M", "s1F3F. M", "s1F2C. M", "s2C3F. f", "s1F1F.;m",
};

unsigned short parse_form(char ch) {
	unsigned short m =
		ch == 'S' ? WAVEFORM_SINE :
		ch == 'Q' ? WAVEFORM_SQUARE :
		ch == 'W' ? WAVEFORM_SAW :
		ch == 'w' ? WAVEFORM_WIGGLE :
		ch == 's' ? WAVEFORM_STEP : 0xFFFF;
	if (m == 0xFFFF)
		m = WAVEFORM_SINE;
	return m;
}

unsigned short parse_pitch(char oct, char note) {
	unsigned short freq = CALC_PITCH(
			oct == '1' ? OCTAVE_L :
			oct == '2' ? OCTAVE_2 :
			oct == '3' ? OCTAVE_3 :
			oct == '4' ? OCTAVE_4 :
			oct == '5' ? OCTAVE_5 :
			oct == '6' ? OCTAVE_6 :
			oct == '7' ? OCTAVE_H : 0
		,
			note == 'C' ? NOTE_C :
			note == 'd' ? NOTEbD :
			note == 'D' ? NOTE_D :
			note == 'e' ? NOTEbE :
			note == 'E' ? NOTE_E :
			note == 'F' ? NOTE_F :
			note == 'g' ? NOTEbG :
			note == 'G' ? NOTE_G :
			note == 'a' ? NOTEbA :
			note == 'A' ? NOTE_A :
			note == 'b' ? NOTEbB :
			note == 'B' ? NOTE_B : 0
		);
	if (freq == 0)
		freq = 0;
	return freq;
}

unsigned short parse_span(char ch) {
	unsigned short len =
		ch == '1' ? 240 :
		ch == '!' ? 360 :
		ch == '2' ? 480 :
		ch == '@' ? 600 :
		ch == '3' ? 720 :
		ch == '4' ? 960 :
		ch == ',' ? 120 :
		ch == ';' ? 180 :
		ch == '.' ? 60 :
		ch == ':' ? 90 :
		ch == '<' ? 80 :
		ch == '>' ? 40 : 0xFFFF;
	if (len == 0xFFFF)
		len = 0;
	return len;
}

unsigned short parse_delta(char ch) {
	unsigned short dlen =
		ch == ' ' ? 0 :
		ch == '.' ? 10 :
		ch == ':' ? 15 :
		ch == ',' ? 20 :
		ch == ';' ? 30 :
		ch == '-' ? 40 :
		ch == '>' ? 60 :
		ch == '=' ? 80 : 0xFFFF;
	if (dlen == 0xFFFF)
		dlen = 0;
	return dlen;
}

short parse_intensity(char ch) {
	short ints =
		ch == 'p' ? INTENSITYpPIANO :
		ch == 'P' ? INTENSITY_PIANO :
		ch == 'm' ? INTENSITYpMEZZO :
		ch == 'M' ? INTENSITY_MEZZO :
		ch == 'f' ? INTENSITY_FORTE :
		ch == 'F' ? INTENSITYfFORTE :
		ch == 'X' ? INTENSITY_MAX : 0x8000;
	if (ints == (short)0x8000)
		ints = INTENSITY_PIANO / 2;
	return ints;
}

void print_data(note_info *pinfo, file_p pf) {
	unsigned char *pb = (unsigned char*)pinfo;
	putchar('\t');
	putchar('{');
	for (unsigned short j = 0; j < sizeof(note_info); j++) {
		printf("0x%02hhX, ", pb[j]);
	}
	puts("},");
	fwrite(pb, sizeof(note_info), 1, pf);
}

int main() {
	file_p pf = fopen("score.dat", "wb");
	puts("notes[] = {");
	for (unsigned short i = 0, off = 0; i < sizeof(notes) / sizeof(note_simplex); i++) {
		note_simplex note = notes[i];
		if (note[0] == 'R' || note[0] == 'r' || note[0] == '-') {
			off += parse_span(note[1] != ' ' ? note[1] : note[3]);
		} else if (note[0] == '<') {
			putchar('\t');
			puts("/* ----- */");
			off = 0;
		} else {
			note_info info;
			unsigned short m = 0, cnt = 1;
			if (note[5] != '\0') {
				for (unsigned short pos = 6; note[pos] != '\0'; pos += 2) {
					cnt++;
				}
				m = cnt;
			}
			info.form = parse_form(note[0]);
			info.intensity = parse_intensity(note[m == 0 ? 4 : 3 + 2 * m]) / cnt;
			info._reserved = 0;
			unsigned short len = parse_span(note[m == 0 ? 3 : 1 + 2 * m]),
				dlen = (m != 0 ? parse_delta(note[2 + 2 * m]) : 0);
			info.offset = _SPAN(off);
			info.length = _BEAT(info.form != WAVEFORM_STEP ? len : 15);
			for (unsigned short j = 0; j < cnt; j++) {
				if (note[2 + 2 * j] == ' ')
					continue;
				info.pitch = parse_pitch(note[1 + 2 * j], note[2 + 2 * j]);
				print_data(&info, pf);
				if (m != 0) {
					info.offset += _SPAN(dlen);
					info.length -= (info.form != WAVEFORM_STEP ? _SPAN(dlen) : 0);
				}
			}
			off += len;
		}
	}
	puts("};");
	fclose(pf);
	return 0;
}
