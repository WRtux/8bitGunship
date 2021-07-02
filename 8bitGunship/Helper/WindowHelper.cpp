#include "Helper.h"
#include "../Resource/Resources.h"

namespace Helper { namespace Window {
	
	static HINSTANCE Instance = NULL;
	
	static PCONTROLPROC ControlProc = NULL;
	static PPAINTPROC PaintProc = NULL;
	
	static LPDLGTEMPLATE DialogTemplate = NULL;
	static HFONT DefaultFont = NULL;
	
	static LONG CALLBACK MainWindowProc(HWND, UINT, WPARAM, LPARAM);
	static INT CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
	
	VOID WINAPI InitWindow(HINSTANCE hins, PCONTROLPROC ctlproc, PPAINTPROC ptproc) {
		if (hins == NULL || ctlproc == NULL || ptproc == NULL)
			ExitProcess(0x434F5250);
		Instance = hins;
		ControlProc = ctlproc;
		PaintProc = ptproc;
		PWNDCLASS pcls = (PWNDCLASS)MemoryAlloc(sizeof(WNDCLASS));
		pcls->style = 0;
		pcls->lpfnWndProc = &MainWindowProc;
		pcls->cbClsExtra = pcls->cbWndExtra = 0;
		pcls->hInstance = Instance;
		pcls->hIcon = LoadIcon(NULL, IDI_SHIELD);
		pcls->hCursor = LoadCursor(NULL, IDC_ARROW);
		pcls->hbrBackground = GetSysColorBrush(COLOR_WINDOW);
		pcls->lpszMenuName = NULL;
		pcls->lpszClassName = L"MainWindow";
		RegisterClass(pcls);
		MemoryFree(pcls);
		DialogTemplate = (LPDLGTEMPLATE)LoadDecompressLZSS(IDR_DIALOGS, NULL);
		DefaultFont = CreateFont(14, 0, 0, 0, 400, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, NULL);
	}
	
	static HWND WINAPI InternalCreate(PTSTR cls, USHORT id, PTSTR name, HWND hpnt, DWORD sty, INT x, INT y, INT w, INT h) {
		HWND hwnd = CreateWindow(cls, name, sty, x, y, w, h, hpnt, (HMENU)id, Instance, NULL);
		PostMessage(hwnd, WM_SETFONT, (DWORD)DefaultFont, FALSE);
		return hwnd;
	}
	
	HWND WINAPI CreateMainWindow(PTSTR name, HWND *phwnd, UINT w, UINT h) {
		HWND hwnd = InternalCreate(
			L"MainWindow", 0, name, NULL,
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			CW_USEDEFAULT, CW_USEDEFAULT, w, h);
		*phwnd = hwnd;
		RECT rect;
		GetClientRect(hwnd, &rect);
		SetWindowSize(hwnd, w + w - rect.right, h + h - rect.bottom, FALSE);
		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);
		return hwnd;
	}
	
	INT WINAPI PopupDialog(HWND hpnt, PVOID dat) {
		return DialogBoxIndirectParam(Instance, DialogTemplate, hpnt, DialogProc, (LONG)dat);
	}
	
	static LONG CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_CREATE:
			return (*ControlProc)(NULL, msg, wParam, lParam);
		case WM_PAINT: {
			PPAINTSTRUCT ppts = (PPAINTSTRUCT)MemoryAlloc(sizeof(PAINTSTRUCT));
			HDC hdc = BeginPaint(hwnd, ppts);
			FillRect(hdc, &ppts->rcPaint, GetSysColorBrush(COLOR_WINDOW));
			(*PaintProc)(hwnd, hdc);
			EndPaint(hwnd, ppts);
			MemoryFree(ppts);
			return 0;
		}
		case WM_DESTROY:
			(*ControlProc)(hwnd, msg, wParam, lParam);
			PostQuitMessage(0);
			return 0;
		default: {
			LONG res = (*ControlProc)(hwnd, msg, wParam, lParam);
			if (res != 0)
				return DefWindowProc(hwnd, msg, wParam, lParam);
			return 0;
		}}
	}
	
	static INT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_INITDIALOG: {
			PTSTR *pdat = (PTSTR*)lParam;
			if (pdat != NULL) {
				SetWindowText(hwnd, pdat[0]);
				SetDlgItemText(hwnd, 0, pdat[1]);
				MemoryFree(pdat);
			}
			EnumChildWindows(hwnd, [](HWND hctl, LPARAM) ->BOOL {
				PostMessage(hctl, WM_SETFONT, (DWORD)DefaultFont, FALSE);
				return TRUE;
			}, 0);
			return TRUE;
		}
		case WM_COMMAND:
			if (lParam != NULL && HIWORD(wParam) == BN_CLICKED)
				EndDialog(hwnd, 0);
			return 0;
		case WM_CLOSE:
			EndDialog(hwnd, 0);
			return 0;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
	
	SIZE WINAPI GetWindowSize(HWND hwnd) {
		RECT rect;
		GetWindowRect(hwnd, &rect);
		SIZE size = {rect.right - rect.left, rect.bottom - rect.top};
		return size;
	}
	
	VOID WINAPI SetWindowSize(HWND hwnd, UINT w, UINT h, BOOL refresh) {
		RECT rect;
		GetWindowRect(hwnd, &rect);
		MoveWindow(hwnd, rect.left, rect.top, w, h, refresh);
	}
	
	HWND WINAPI AddControl(PTSTR cls, USHORT id, PTSTR name, HWND hpnt, DWORD sty, INT x, INT y, UINT w, UINT h) {
		return InternalCreate(cls, id, name, hpnt, WS_CHILD | WS_VISIBLE | sty, x, y, w, h);
	}
	
	VOID WINAPI DrawImage(HDC hdc, PIMAGE pimg, SHORT x, SHORT y, USHORT s) {
		HDC hbmpdc = CreateCompatibleDC(hdc);
		SelectObject(hbmpdc, pimg->hbitmap);
		if (s != 0) {
			StretchBlt(hdc, x, y, pimg->w * s, pimg->h * s, hbmpdc, 0, 0, pimg->w, pimg->h, SRCCOPY);
		} else {
			BitBlt(hdc, x, y, pimg->w, pimg->h, hbmpdc, 0, 0, SRCCOPY);
		}
		DeleteDC(hbmpdc);
	}
	VOID WINAPI DrawImageCenter(HDC hdc, PIMAGE pimg, SHORT x, SHORT y, USHORT s) {
		DrawImage(hdc, pimg, x - pimg->w * s / 2, y - pimg->h * s / 2, s);
	}
	
}}
