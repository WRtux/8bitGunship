#include "Game.h"

VOID WINAPI Init(VOID) {
	if (!CheckSIMD()) {
		MessageBoxA(NULL, "SSE not supported by CPU.", "Failure", MB_ICONERROR | MB_OK);
		ExitProcess(0x444D4953);
	}
	if (LOBYTE(GetVersion()) < 0x06) {
		MessageBoxA(NULL, "Windows version too low.", "Warning", MB_ICONWARNING | MB_OK);
	} else {
		SetProcessDPIAware();
	}
	BOOL f = FALSE;
	SetUserObjectInformation(GetCurrentProcess(), 7, &f, sizeof(BOOL));
	CreateHelperThread();
	StartHelperThread();
}

extern "C"
INT WINAPI Main(HINSTANCE hins, HINSTANCE, PTSTR cmdln, INT show) {
	UNREFERENCED_PARAMETER(show);
	Init();
	InitGame(hins);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	CleanupGame(hins);
	ExitHelperThread();
	return msg.wParam;
}
