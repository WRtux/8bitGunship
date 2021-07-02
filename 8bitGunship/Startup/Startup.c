#include "StartupSupport.h"

extern VOID __security_init_cookie(VOID);
extern const PIMAGE_TLS_CALLBACK __dyn_tls_init_callback;

static INT CDECL CInit(VOID);
static VOID CDECL CPPInit(VOID);

allocate(".CRT$XIA") static const PINT_STARTER xi_a = &CInit;
allocate(".CRT$XIZ") static const PINT_STARTER xi_z = NULL;

allocate(".CRT$XCA") static const PVOID_STARTER xc_a = &CPPInit;
allocate(".CRT$XCZ") static const PVOID_STARTER xc_z = NULL;

INT WinStartup(VOID) {
	PTSTR cmdln;
	DWORD stfs;
	WORD show;
	INT ret;
	
#	ifdef COOKIE
	__security_init_cookie();
#	endif
#	ifdef HEAPSEC
	if (!_NoHeapEnableTerminationOnCorruption)
		HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#	endif
	
	SetUnhandledExceptionFilter(&FailureHandler); // try...
	
	if (InitTermsReturn(&xi_a, &xi_z) != 0)
		return 0xFF;
	InitTerms(&xc_a, &xc_z);
#	ifdef TLS
	__dyn_tls_init_callback(NULL, DLL_THREAD_ATTACH, NULL);
#	endif
	
	{ // skim command head
		BOOL inquote = FALSE;
		cmdln = GetCommandLine();
		while (*cmdln > L' ' || (*cmdln && inquote)) {
			if (*cmdln == L'"')
				inquote = !inquote;
			cmdln++;
		}
		while (*cmdln && *cmdln <= L' ') {
			cmdln++;
		}
	}{ // get show info
		HANDLE heap = GetProcessHeap();
		LPSTARTUPINFO pinfo = (LPSTARTUPINFO)HeapAlloc(heap, 0, sizeof(STARTUPINFO));
		if (pinfo == NULL)
			ExitProcess(GetLastError());
		GetStartupInfo(pinfo);
		stfs = pinfo->dwFlags;
		show = pinfo->wShowWindow;
		if (!HeapFree(heap, 0, pinfo))
			ExitProcess(GetLastError());
	}
	
	ret = Main(HANDLE_INSTANCE, NULL, cmdln, stfs & STARTF_USESHOWWINDOW ? show : SW_SHOWDEFAULT);
	
	return ret;
}

static INT CDECL CInit(VOID) {
	// add init code here
	return 0;
}

static VOID CDECL CPPInit(VOID) {
	// add init code here
}
