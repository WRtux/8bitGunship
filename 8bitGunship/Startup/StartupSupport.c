#include "StartupSupport.h"

#pragma comment(linker, "/merge:.CRT=.rdata")

VOID WINAPI InitTerms(PCPVOID_STARTER st, PCPVOID_STARTER ed) {
	while (st < ed) {
		if (*st != NULL)
			(**st)();
		st++;
	}
}

INT WINAPI InitTermsReturn(PCPINT_STARTER st, PCPINT_STARTER ed) {
	INT ret = 0;
	while (st < ed && ret == 0) {
		if (*st != NULL)
			ret = (**st)();
		st++;
	}
	return ret;
}

LONG WINAPI FailureHandler(PEXCEPTION_POINTERS pinfo) {
	Beep(800, 100);
	ExitProcess(pinfo->ExceptionRecord->ExceptionCode);
}
