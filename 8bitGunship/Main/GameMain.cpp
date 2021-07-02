#include "Game.h"

#define IMAGE_COUNT 6
#define SCORE_COUNT 2
#define CONTROL_COUNT 3

extern IMAGE Images[IMAGE_COUNT] = {};
extern PSCOREINFO Scores[SCORE_COUNT] = {};

static HWND MainWindowHandle = NULL;
static HWND ControlHandles[CONTROL_COUNT] = {};

extern HWAVEOUT AudioHandle = NULL;

static LONG CALLBACK GameControlProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_CREATE) {
		LPCREATESTRUCT pcrts = (LPCREATESTRUCT)lParam;
		UNREFERENCED_PARAMETER(pcrts);
		return 0;
	}
	if (hwnd == MainWindowHandle) {
		switch (msg) {
		case WM_COMMAND:
			if (lParam != NULL && HIWORD(wParam) == BN_CLICKED) {
				switch (LOWORD(wParam)) {
				case 0: {
					PTSTR *pdat = (PTSTR*)MemoryAlloc(sizeof(PTSTR) * 2);
					pdat[0] = Vocabulary[VOCAB_INFO_HELP_TITLE];
					pdat[1] = Vocabulary[VOCAB_INFO_HELP];
					PopupDialog(hwnd, pdat);
					for (USHORT i = 0; i < CONTROL_COUNT; i++) {
						ShowWindow(ControlHandles[i], SW_HIDE);
					}
					StartGameplay();
					break;
				}
				case 1: {
					PTSTR *pdat = (PTSTR*)MemoryAlloc(sizeof(PTSTR) * 2);
					pdat[0] = Vocabulary[VOCAB_MAIN_ABOUT];
					pdat[1] = Vocabulary[VOCAB_INFO_ABOUT];
					PopupDialog(hwnd, pdat);
					break;
				}
				case 2:
					DestroyWindow(hwnd);
					break;
				}
			}
			return 0;
		case WM_KEYDOWN:
		case WM_KEYUP:
			switch (wParam) {
			case VK_LEFT:
			case VK_RIGHT:
				KeyStates[wParam == VK_LEFT ? 0 : 1] = (msg == WM_KEYDOWN);
				return 0;
			case VK_UP:
			case VK_DOWN:
				KeyStates[wParam == VK_UP ? 2 : 3] = (msg == WM_KEYDOWN);
				return 0;
			case VK_SPACE:
			case VK_CONTROL:
				KeyStates[4] = (msg == WM_KEYDOWN);
				return 0;
			default:
				return -1;
			}
		case WM_KILLFOCUS:
			for (USHORT i = 0; i < 5; i++) {
				KeyStates[i] = FALSE;
			}
			return 0;
		default:
			return -1;
		}
	} else {
		return -1;
	}
}

static VOID CALLBACK GameUpdateProc(HWND hwnd, UINT, UINT id, ULONG t) {
	UNREFERENCED_PARAMETER(t);
	if (hwnd == MainWindowHandle && id == 1) {
		if (GameState > 0) {
			UpdateGameplay();
			HDC hdc = GetDC(hwnd);
			PaintGameplay(hdc);
			ReleaseDC(hwnd, hdc);
			if (GameState == 0) {
				PTSTR *pdat = (PTSTR*)MemoryAlloc(sizeof(PTSTR) * 2);
				PTSTR str = (PTSTR)MemoryAlloc(0x100);
				SimpleDecimal(str + StringCopy(str, Vocabulary[VOCAB_INFO_GAMEOVER], 0x100), GameScore);
				pdat[0] = Vocabulary[VOCAB_INFO_GAMEOVER_TITLE];
				pdat[1] = str;
				PopupDialog(hwnd, pdat);
				MemoryFree(str);
				StartGameplay();
			}
		}
	}
}

static VOID CALLBACK GamePaintProc(HWND hwnd, HDC hdc) {
	if (hwnd == MainWindowHandle) {
		if (GameState > 0) {
			PaintGameplay(hdc);
		} else {
			DrawImageCenter(hdc, &Images[1], 240, 240, 8);
			DrawImageCenter(hdc, &Images[4], 400, 240, 8);
		}
	}
}

VOID WINAPI InitGame(HINSTANCE hins) {
	BuildVocabulary((PTSTR)LoadDecompressLZSS(IDR_STRINGS, NULL));
	PWORD ptr = (PWORD)LoadDecompressLZSS(IDR_IMAGES, NULL);
	for (USHORT i = 0; i < IMAGE_COUNT; i++) {
		Images[i].w = *ptr++;
		Images[i].h = *ptr++;
		Images[i].hbitmap = CreateBitmap(Images[i].w, Images[i].h, 1, 1, ptr);
		ptr += (Images[i].w + 15) / 16 * Images[i].h;
	}
	InitWindow(hins, &GameControlProc, &GamePaintProc);
	HWND hwnd = CreateMainWindow(Vocabulary[VOCAB_MAIN_TITLE], &MainWindowHandle, 640, 480);
	ControlHandles[0] = AddControl(L"BUTTON", 0, Vocabulary[VOCAB_MAIN_START], hwnd, BS_PUSHBUTTON, 192, 440, 80, 24);
	ControlHandles[1] = AddControl(L"BUTTON", 1, Vocabulary[VOCAB_MAIN_ABOUT], hwnd, BS_PUSHBUTTON, 280, 440, 80, 24);
	ControlHandles[2] = AddControl(L"BUTTON", 2, Vocabulary[VOCAB_MAIN_EXIT], hwnd, BS_PUSHBUTTON, 368, 440, 80, 24);
	SetTimer(hwnd, 1, 1000 / 60, &GameUpdateProc);
	AudioHandle = InitAudio();
	if (AudioHandle == NULL) {
		MessageBoxA(NULL, "Audio failed!", "Error", MB_ICONERROR | MB_OK);
	} else {
		InitWaveData();
		Scores[0] = (PSCOREINFO)LoadDecompressLZSS(IDR_SCORES, NULL);
		Scores[1] = (PSCOREINFO)&Scores[0]->notes[Scores[0]->count];
		PlayScore(AudioHandle, GenerateScore(Scores[0], 0, 0), 0xFFFF);
	}
}

VOID WINAPI CleanupGame(HINSTANCE) {
	KillTimer(MainWindowHandle, 1);
	if (AudioHandle != NULL)
		CloseAudio(AudioHandle);
}
