#include "Helper.h"

namespace Helper {
	namespace Audio {
		extern VOID CALLBACK AudioProc(HWAVEOUT, PWAVEHDR, UINT);
	}
	using namespace Audio;
	
	extern const UINT RANDOM_SEED = GetTickCount();
	
	static HANDLE HelperThreadHandle = NULL;
	extern DWORD HelperThreadId = 0;
	
	static DWORD CALLBACK HelperProc(PVOID);
	
	BOOL WINAPI CreateHelperThread(VOID) {
		if (HelperThreadHandle == NULL) {
			HelperThreadHandle = CreateThread(NULL, 0, &HelperProc, NULL, CREATE_SUSPENDED, &HelperThreadId);
			if (HelperThreadHandle == NULL)
				ExitProcess(GetLastError());
			return TRUE;
		} else {
			return FALSE;
		}
	}
	
	BOOL WINAPI StartHelperThread(VOID) {
		if (HelperThreadHandle != NULL) {
			INT cnt = ResumeThread(HelperThreadHandle);
			if (cnt == -1)
				ExitProcess(GetLastError());
			if (cnt == 1)
				SwitchToThread();
			return cnt == 1;
		} else {
			return FALSE;
		}
	}
	
	BOOL WINAPI ExitHelperThread(VOID) {
		if (HelperThreadHandle != NULL) {
			return PostThreadMessage(HelperThreadId, WM_QUIT, 0, 0);
		} else {
			return FALSE;
		}
	}
	
	static DWORD CALLBACK HelperProc(PVOID param) {
		UNREFERENCED_PARAMETER(param);
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0) > 0) {
			switch (msg.message) {
			case WM_TIMER:
				break;
			case MM_WOM_DONE:
				AudioProc((HWAVEOUT)msg.wParam, (PWAVEHDR)msg.lParam, msg.message);
				break;
			default:
				OutputDebugStringA("Unrecognizable message.");
			}
		}
		return msg.wParam;
	}
	
}
