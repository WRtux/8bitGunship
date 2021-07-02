#pragma once

#include <Windows.h>

#ifdef NDEBUG
#define DebugBreak __noop
#define OutputDebugStringA __noop
#define OutputDebugStringW __noop
#endif

namespace Helper {
	
	extern const UINT RANDOM_SEED;
	
	extern BOOL WINAPI CreateHelperThread(VOID);
	extern BOOL WINAPI StartHelperThread(VOID);
	extern BOOL WINAPI ExitHelperThread(VOID);
	
	extern PVOID WINAPI MemoryAlloc(UINT);
	extern VOID WINAPI MemoryFree(PVOID);
	extern PVOID WINAPI MemoryClear(PVOID, UINT);
	extern PVOID WINAPI MemoryFill(PVOID, BYTE, UINT);
	extern PVOID WINAPI MemoryCopy(PVOID, PVOID, UINT);
	extern PBYTE WINAPI MemoryAdd(PBYTE, BYTE, UINT);
	
	extern PTSTR WINAPI StringAlloc(USHORT);
	extern USHORT WINAPI StringCopy(PTSTR, PTSTR, USHORT);
	extern USHORT WINAPI StringAppend(PTSTR, PTSTR, USHORT);
	extern USHORT WINAPI SimpleDecimal(PTSTR, INT);
	extern USHORT WINAPI SimpleHexadecimal(PTSTR, UINT);
	extern USHORT WINAPI FormatDecimal(PTSTR, UINT, TCHAR);
	
	extern const FLOAT PI;
	extern const FLOAT E;
	
	extern BOOL WINAPI CheckSIMD(VOID);
	extern PINT WINAPI FloorFloatSIMD(PFLOAT, PINT);
	extern PSHORT WINAPI FloorFloatSIMD(PFLOAT, PSHORT);
	extern INT WINAPI RoundFloat(FLOAT);
	extern PINT WINAPI RoundFloatSIMD(PFLOAT, PINT);
	extern PSHORT WINAPI RoundFloatSIMD(PFLOAT, PSHORT);
	extern USHORT WINAPI NextRandom(USHORT lim);
	
	extern PUCHAR WINAPI AddSatUCharSIMD(PUCHAR, PUCHAR);
	extern PCHAR WINAPI AddSatCharSIMD(PCHAR, PCHAR);
	extern PFLOAT WINAPI MultiplyFloatSIMD(PFLOAT, FLOAT);
	extern PFLOAT WINAPI MultiplyFloatSIMD(PFLOAT, PFLOAT);
	extern PFLOAT WINAPI SinrSIMD(PFLOAT, PFLOAT);
	
	namespace Window {
		
		typedef LONG (CALLBACK *PCONTROLPROC)(HWND, UINT, WPARAM, LPARAM);
		typedef VOID (CALLBACK *PPAINTPROC)(HWND, HDC);
		typedef struct {
			USHORT w, h;
			HBITMAP hbitmap;
		} IMAGE, *PIMAGE;
		
		extern VOID WINAPI InitWindow(HINSTANCE, PCONTROLPROC, PPAINTPROC);
		extern HWND WINAPI CreateMainWindow(PTSTR, HWND*, UINT, UINT);
		extern INT WINAPI PopupDialog(HWND, PVOID);
		extern SIZE WINAPI GetWindowSize(HWND);
		extern VOID WINAPI SetWindowSize(HWND, UINT, UINT, BOOL);
		extern HWND WINAPI AddControl(PTSTR, USHORT, PTSTR, HWND, DWORD, INT, INT, UINT, UINT);
		extern VOID WINAPI DrawImage(HDC, PIMAGE, SHORT, SHORT, USHORT);
		extern VOID WINAPI DrawImageCenter(HDC, PIMAGE, SHORT, SHORT, USHORT);
		
	}
	
	namespace Audio {
		
#		define SAMPLE_RATE 22000
		
		typedef struct {
			UINT length;
			PBYTE *slices;
		} SCORE, *PSCORE;
		
		extern HWAVEOUT WINAPI InitAudio(VOID);
		extern VOID WINAPI CloseAudio(HWAVEOUT);
		extern VOID WINAPI PlayScore(HWAVEOUT, PSCORE, USHORT);
		extern VOID WINAPI ClearPlayback(HWAVEOUT);
		
#		define WAVEFORM_SINE 0
#		define WAVEFORM_SQUARE 1
#		define WAVEFORM_SAW 2
#		define WAVEFORM_WIGGLE 4
#		define WAVEFORM_STEP 6
		
#		define OCTAVE_L 1
#		define OCTAVE_2 2
#		define OCTAVE_3 4
#		define OCTAVE_4 8
#		define OCTAVE_5 16
#		define OCTAVE_6 32
#		define OCTAVE_H 64
#		define NOTE_C 261.63f
#		define NOTEbD 277.18f
#		define NOTE_D 293.66f
#		define NOTEbE 311.13f
#		define NOTE_E 329.63f
#		define NOTE_F 349.23f
#		define NOTEbG 369.99f
#		define NOTE_G 392.00f
#		define NOTEbA 415.30f
#		define NOTE_A 440.00f
#		define NOTEbB 466.16f
#		define NOTE_B 493.88f
		
#		define CALC_PITCH(oct, note) (USHORT)((USHORT)(oct) * (note) / 8 + 0.5f)
		
#		define LENGTH_BEAT 240
#		define LENGTH_GAP 10
		
#		define CALC_SPAN(spd, len) (UINT)(SAMPLE_RATE * (USHORT)(len) / (spd) * 60 / LENGTH_BEAT)
#		define CALC_BEAT(spd, len) CALC_SPAN(spd, (len) - LENGTH_GAP)
		
#		define INTENSITYpPIANO 0x0BFF
#		define INTENSITY_PIANO 0x13FF
#		define INTENSITYpMEZZO 0x1FFF
#		define INTENSITY_MEZZO 0x2BFF
#		define INTENSITY_FORTE 0x3FFF
#		define INTENSITYfFORTE 0x57FF
#		define INTENSITY_MAX 0x7FFF
		
		typedef struct {
			USHORT form;
			USHORT pitch;
			SHORT intensity;
			UINT offset;
			UINT length;
		} NOTEINFO, *PNOTEINFO;
		typedef const NOTEINFO *PCNOTEINFO;
		typedef struct {
			UINT count;
			NOTEINFO notes[1];
		} SCOREINFO, *PSCOREINFO;
		typedef const SCOREINFO *PCSCOREINFO;
		
		extern VOID WINAPI InitWaveData(VOID);
		extern UINT WINAPI CalculateScoreSize(PCNOTEINFO, UINT);
		extern PBYTE WINAPI GenerateScoreSlice(PCNOTEINFO, UINT, PVOID, UINT, UINT);
		extern PSCORE WINAPI GenerateScore(PCNOTEINFO, UINT, UINT, UINT);
		extern PSCORE WINAPI GenerateScore(PCSCOREINFO, UINT, UINT);
		
	}
	
}
