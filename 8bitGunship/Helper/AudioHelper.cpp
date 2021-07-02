#include "Helper.h"

namespace Helper {
	extern DWORD HelperThreadId;
namespace Audio {
	
	struct {
		USHORT count;
		PWAVEHDR *headers;
	} CurrentPlayback;
	
	HWAVEOUT WINAPI InitAudio(VOID) {
		HWAVEOUT hwo = NULL;
		PWAVEFORMATEX pwfmt = (PWAVEFORMATEX)MemoryAlloc(sizeof(WAVEFORMATEX));
		pwfmt->wFormatTag = WAVE_FORMAT_PCM;
		pwfmt->nChannels = 1;
		pwfmt->nSamplesPerSec = SAMPLE_RATE;
		pwfmt->nAvgBytesPerSec = SAMPLE_RATE;
		pwfmt->nBlockAlign = 1;
		pwfmt->wBitsPerSample = 8;
		pwfmt->cbSize = 0;
		if (HelperThreadId == 0)
			CreateHelperThread();
		UINT res = waveOutOpen(&hwo, WAVE_MAPPER, pwfmt, HelperThreadId, 0, CALLBACK_THREAD);
		MemoryFree(pwfmt);
		SetLastError(res);
		return hwo;
	}
	
	VOID WINAPI CloseAudio(HWAVEOUT hwo) {
		if (CurrentPlayback.headers != NULL)
			ClearPlayback(hwo);
		SetLastError(waveOutClose(hwo));
	}
	
	VOID WINAPI PlayScore(HWAVEOUT hwo, PSCORE pscr, USHORT lps) {
		if (CurrentPlayback.headers != NULL)
			ClearPlayback(hwo);
		if (pscr == NULL)
			return;
		USHORT cnt = (USHORT)((pscr->length + SAMPLE_RATE - 1) / SAMPLE_RATE);
		UINT len = (pscr->length - 1) % SAMPLE_RATE + 1;
		CurrentPlayback.count = cnt;
		CurrentPlayback.headers = (PWAVEHDR*)MemoryAlloc(sizeof(PWAVEHDR) * cnt);
		for (USHORT i = 0; i < cnt; i++) {
			PWAVEHDR pwhdr = (PWAVEHDR)MemoryAlloc(sizeof(WAVEHDR));
			pwhdr->lpData = (PSTR)pscr->slices[i];
			pwhdr->dwBufferLength = (i == cnt - 1 ? len : SAMPLE_RATE);
			if (lps != 0) {
				pwhdr->dwFlags = (i == 0 ? WHDR_BEGINLOOP : i == cnt - 1 ? WHDR_ENDLOOP : 0);
			} else {
				pwhdr->dwFlags = 0;
			}
			pwhdr->dwLoops = lps;
			pwhdr->lpNext = NULL;
			pwhdr->dwUser = NULL;
			if (lps == 0 && i == cnt - 1)
				pwhdr->dwUser = (DWORD)pscr;
			CurrentPlayback.headers[i] = pwhdr;
			UINT res = waveOutPrepareHeader(hwo, pwhdr, sizeof(WAVEHDR));
			if (res == 0)
				res = waveOutWrite(hwo, pwhdr, sizeof(WAVEHDR));
			if (res != 0) {
				waveOutReset(hwo);
				for (USHORT j = 0; j <= i; j++) {
					waveOutUnprepareHeader(hwo, CurrentPlayback.headers[j], sizeof(WAVEHDR));
					MemoryFree(CurrentPlayback.headers[j]);
				}
				MemoryFree(CurrentPlayback.headers);
				CurrentPlayback.count = 0;
				CurrentPlayback.headers = NULL;
				SetLastError(res);
				return;
			}
		}
		SetLastError(NO_ERROR);
	}
	
	VOID WINAPI ClearPlayback(HWAVEOUT hwo) {
		waveOutReset(hwo);
		for (USHORT i = 0; i < CurrentPlayback.count; i++) {
			PWAVEHDR pwhdr = CurrentPlayback.headers[i];
			waveOutUnprepareHeader(hwo, pwhdr, sizeof(WAVEHDR));
			MemoryFree(pwhdr->lpData);
			MemoryFree(pwhdr);
		}
		MemoryFree(CurrentPlayback.headers);
		CurrentPlayback.count = 0;
		CurrentPlayback.headers = NULL;
	}
	
	VOID CALLBACK AudioProc(HWAVEOUT hwo, PWAVEHDR pwhdr, UINT msg) {
		if (msg == MM_WOM_DONE) {
			if (pwhdr->dwUser != NULL) {
				ClearPlayback(hwo);
				((PSCORE)pwhdr->dwUser)->slices = NULL;
			}
		}
	}
	
#	define WAVEDATA_SIZE (0x100 / 2)
#	define SINE_INTENSITY 0x7FFF
#	define SQUARE_INTENSITY 0x4FFF
#	define SAW_INTENSITY 0x5FFF
	
#	define SCALE_PHASE(num) ((FLOAT)(num) / WAVEDATA_SIZE * 2)
#	define SCALE_PITCH(ph, size) ((ph) % SAMPLE_RATE * (size) / SAMPLE_RATE)
	
	static PSHORT WAVEDATA[WAVEFORM_STEP + 1];
	
	VOID WINAPI InitWaveData(VOID) {
		WAVEDATA[WAVEFORM_SINE] = (PSHORT)MemoryAlloc(sizeof(SHORT) * WAVEDATA_SIZE);
		WAVEDATA[WAVEFORM_WIGGLE] = (PSHORT)MemoryAlloc(sizeof(SHORT) * WAVEDATA_SIZE);
		WAVEDATA[WAVEFORM_STEP] = (PSHORT)MemoryAlloc(sizeof(SHORT) * WAVEDATA_SIZE);
		{ // sine wave
			PSHORT dat = WAVEDATA[WAVEFORM_SINE];
			for (USHORT pos = 0; pos < WAVEDATA_SIZE / 2; pos += 4) {
				FLOAT r[] = {
					SCALE_PHASE(pos), SCALE_PHASE(pos + 1), SCALE_PHASE(pos + 2), SCALE_PHASE(pos + 3)};
				SinrSIMD(r, r);
				MultiplyFloatSIMD(r, (FLOAT)SINE_INTENSITY);
				RoundFloatSIMD(r, &dat[pos]);
			}
			dat[WAVEDATA_SIZE / 2] = SINE_INTENSITY;
			for (USHORT pos = 1; pos < WAVEDATA_SIZE / 2; pos++) {
				dat[WAVEDATA_SIZE / 2 + pos] = dat[WAVEDATA_SIZE / 2 - pos];
			}
		}{ // wiggle wave
			PSHORT dat = WAVEDATA[WAVEFORM_WIGGLE];
			UNREFERENCED_PARAMETER(dat);
			//TODO
		}{ // step noise
			PSHORT dat = WAVEDATA[WAVEFORM_STEP];
			UINT tmp = 1;
			for (USHORT pos = 0; pos + 1 < WAVEDATA_SIZE; pos += 2) {
				tmp = tmp * 214013 + 2531011;
				dat[pos] = dat[pos + 1] = WAVEDATA[WAVEFORM_SINE][pos] / 2 + (SHORT)(tmp >> 8) % 0x4000;
			}
		}
	}
	
	UINT WINAPI CalculateScoreSize(PCNOTEINFO pnote, UINT cnt) {
		UINT size = 0;
		for (UINT i = 0; i < cnt; i++) {
			UINT ed = pnote[i].offset + pnote[i].length;
			if (ed > size)
				size = ed;
		}
		return size;
	}
	
	VOID WINAPI MixNoteData(PVOID buf, UINT off, UINT size, PCNOTEINFO pnote) {
		PCHAR dat = (PCHAR)buf;
		UINT st = max(pnote->offset, off), ed = min(pnote->offset + pnote->length, off + size);
		for (UINT i = st; i + 16 <= ed; i += 16) {
			CHAR phs[16];
			switch (pnote->form) {
			case WAVEFORM_SINE:
			case WAVEFORM_WIGGLE:
			case WAVEFORM_STEP:
				for (USHORT j = 0; j < 16; j++) {
					SHORT ph = SCALE_PITCH((i + j) * pnote->pitch, WAVEDATA_SIZE * 2);
					ph = (ph < WAVEDATA_SIZE ? WAVEDATA[pnote->form][ph] : -WAVEDATA[pnote->form][WAVEDATA_SIZE * 2 - ph]);
					ph = ph * pnote->intensity / MAXSHORT;
					phs[j] = (CHAR)(ph >> 8);
				}
				break;
			case WAVEFORM_SQUARE:
				for (USHORT j = 0; j < 16; j++) {
					SHORT ph = SCALE_PITCH((i + j) * pnote->pitch, 2);
					ph = (ph == 0 ? SQUARE_INTENSITY : -SQUARE_INTENSITY);
					ph = ph * pnote->intensity / MAXSHORT;
					phs[j] = (CHAR)(ph >> 8);
				}
				break;
			case WAVEFORM_SAW:
				for (USHORT j = 0; j < 16; j++) {
					SHORT ph = SAW_INTENSITY - SCALE_PITCH((i + j) * pnote->pitch, SAW_INTENSITY * 2);
//					INT ph = SCALE_PITCH((i + j) * pnote->pitch, SAW_INTENSITY * 4);
//					ph = (ph >= SAW_INTENSITY * 3 ? ph - SAW_INTENSITY * 4 : ph);
//					ph = (ph >= SAW_INTENSITY ? SAW_INTENSITY * 2 - ph : ph);
					ph = ph * pnote->intensity / MAXSHORT;
					phs[j] = (CHAR)(ph >> 8);
				}
				break;
			default:
				OutputDebugStringA("Unrecognizable waveform.");
			}
			if (ed - i - 16 < 16) {
				for (USHORT j = 0; j < 16; j++) {
					phs[j] = (CHAR)(phs[j] * (16 - j) / 16);
				}
			}
			AddSatCharSIMD(&dat[i - off], phs);
		}
	}
	
	PBYTE WINAPI GenerateScoreSlice(PCNOTEINFO pnote, UINT cnt, PVOID buf, UINT off, UINT size) {
		if (size == 0)
			size = CalculateScoreSize(pnote, cnt) - off;
		if (buf == NULL)
			buf = MemoryClear(NULL, size);
		for (UINT pos = 0; pos < cnt; pos++, pnote++) {
			if (pnote->offset + pnote->length <= off || pnote->offset >= off + size)
				continue;
			MixNoteData(buf, off, size, pnote);
		}
		return MemoryAdd((PBYTE)buf, 0x80, size);
	}
	
	PSCORE WINAPI GenerateScore(PCNOTEINFO pnote, UINT ncnt, UINT off, UINT size) {
		PSCORE pscr = (PSCORE)MemoryAlloc(sizeof(SCORE));
		if (size == 0)
			size = CalculateScoreSize(pnote, ncnt) - off;
		pscr->length = size;
		USHORT scnt = (USHORT)((size + SAMPLE_RATE - 1) / SAMPLE_RATE);
		pscr->slices = (PBYTE*)MemoryAlloc(sizeof(PBYTE) * scnt);
		for (USHORT i = 0; i < scnt; i++) {
			USHORT slen = (i == scnt - 1 ? (size - 1) % SAMPLE_RATE + 1 : SAMPLE_RATE);
			pscr->slices[i] = (PBYTE)MemoryClear(NULL, slen);
			GenerateScoreSlice(pnote, ncnt, pscr->slices[i], off + SAMPLE_RATE * i, slen);
		}
		return pscr;
	}
	PSCORE WINAPI GenerateScore(PCSCOREINFO pscrinfo, UINT off, UINT size) {
		return GenerateScore(pscrinfo->notes, pscrinfo->count, off, size);
	}
	
}}
