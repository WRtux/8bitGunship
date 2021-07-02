#pragma once

#include <Windows.h>

#pragma section(".CRT$XIA", long, read)
#pragma section(".CRT$XIC", long, read)
#pragma section(".CRT$XID", long, read)
#pragma section(".CRT$XIY", long, read)
#pragma section(".CRT$XIZ", long, read)

#pragma section(".CRT$XCA", long, read)
#pragma section(".CRT$XCC", long, read)
#pragma section(".CRT$XCZ", long, read)

#define allocate(x) __declspec(allocate(x))

typedef VOID (CDECL *PVOID_STARTER)(VOID);
typedef INT (CDECL *PINT_STARTER)(VOID);
typedef PVOID_STARTER *PPVOID_STARTER;
typedef PINT_STARTER *PPINT_STARTER;
typedef const PVOID_STARTER *PCPVOID_STARTER;
typedef const PINT_STARTER *PCPINT_STARTER;

extern const IMAGE_DOS_HEADER __ImageBase;
#define HANDLE_INSTANCE ((HINSTANCE)&__ImageBase)

extern VOID WINAPI InitTerms(PCPVOID_STARTER, PCPVOID_STARTER);
extern INT WINAPI InitTermsReturn(PCPINT_STARTER, PCPINT_STARTER);
extern LONG WINAPI FailureHandler(PEXCEPTION_POINTERS);

extern INT WINAPI Main(HINSTANCE, HINSTANCE, PTSTR, INT);
