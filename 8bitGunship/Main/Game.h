#pragma once

#include "../Helper/Helper.h"
#include "../Resource/Resources.h"
using namespace Helper;
using namespace Helper::Audio;
using namespace Helper::Window;

extern IMAGE Images[];
extern PSCOREINFO Scores[];

extern HWAVEOUT AudioHandle;

extern VOID WINAPI InitGame(HINSTANCE);
extern VOID WINAPI CleanupGame(HINSTANCE);

extern const USHORT GameplayWidth, GameplayHeight;
extern UINT GameState;
extern UINT GameScore;
extern BOOL KeyStates[];

extern VOID WINAPI StartGameplay(VOID);
extern VOID WINAPI UpdateGameplay(VOID);
extern VOID WINAPI PaintGameplay(HDC);
