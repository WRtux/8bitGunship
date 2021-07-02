#include "Game.h"

typedef struct {
	USHORT type;
	SHORT x, y;
	SHORT dx, dy;
	USHORT health;
} PLANEINFO, *PPLANEINFO;
typedef const PLANEINFO *PCPLANEINFO;
typedef struct {
	SHORT x, y;
	SHORT dx, dy;
	USHORT damage;
} BULLETINFO, *PBULLETINFO;

typedef struct _PLANEENTRY {
	PLANEINFO plane;
	_PLANEENTRY *prev, *next;
} PLANEENTRY, *PPLANEENTRY;
typedef struct _BULLETENTRY {
	BULLETINFO bullet;
	_BULLETENTRY *prev, *next;
} BULLETENTRY, *PBULLETENTRY;

extern const USHORT GameplayWidth = 640, GameplayHeight = 480;
static const PLANEINFO PlaneTypes[] = {
	{1, 26, 26, 80, 1, 600},
	{2, 10, 10, 50, -1, 200},
	{3, 18, 18, 100, -1, 300},
	{4, 26, 26, 80, 0, 600},
	{5, 20, 20, 0, -10, 80}
};

static PPLANEINFO WINAPI GeneratePlaneInfo(USHORT typ, PPLANEINFO buf) {
	if (buf == NULL)
		buf = (PPLANEINFO)MemoryAlloc(sizeof(PLANEINFO));
	buf->type = typ;
	if (typ != 0) {
		buf->x = NextRandom(GameplayWidth - PlaneTypes[typ].x);
		buf->y = NextRandom(GameplayHeight / 4);
		buf->dx = NextRandom(5) - 2;
		buf->dy = (NextRandom(3) + 1) / 2;
	} else {
		buf->x = (GameplayWidth - PlaneTypes[0].x) / 2;
		buf->y = GameplayHeight - 80;
		buf->dx = buf->dy = 0;
	}
	buf->health = PlaneTypes[typ].health;
	return buf;
}

static PBULLETINFO WINAPI GenerateBulletInfo(PPLANEINFO pp, BOOL up, PBULLETINFO buf) {
	if (buf == NULL)
		buf = (PBULLETINFO)MemoryAlloc(sizeof(BULLETINFO));
	buf->x = pp->x + PlaneTypes[pp->type].x / 2;
	buf->y = pp->y + (up ? 0 : PlaneTypes[pp->type].y);
	buf->dx = (pp->dx == 0 ? 0 : pp->dx > 0 ? 1 : -1);
	buf->dy = (up ? -6 : 6) + (pp->dy == 0 ? 0 : pp->dy > 0 ? 1 : -1);
	buf->damage = PlaneTypes[pp->type].dx;
	return buf;
}

static VOID WINAPI CheckEdge(PPLANEINFO pp) {
	PCPLANEINFO ppt = &PlaneTypes[pp->type];
	if (pp->x < 0) {
		pp->x = 0;
		pp->dx = -pp->dx;
	} else if (pp->x + ppt->x > GameplayWidth) {
		pp->x = GameplayWidth - ppt->x;
		pp->dx = -pp->dx;
	}
	if (pp->y < 0) {
		pp->y = 0;
		pp->dy = -pp->dy;
	} else if (pp->y + ppt->y > GameplayHeight) {
		pp->y = GameplayHeight - ppt->y;
		pp->dy = -pp->dy;
	}
}

static VOID WINAPI CheckEdge(PBULLETINFO pb) {
	if (pb->x < 0 || pb->x >= GameplayWidth || pb->y < 0 || pb->y >= GameplayHeight)
		pb->damage = 0;
}

static BOOL WINAPI CheckCollide(PPLANEINFO ppa, PPLANEINFO ppb) {
	return ppa->x + PlaneTypes[ppa->type].x > ppb->x && ppa->x < ppb->x + PlaneTypes[ppb->type].x &&
		ppa->y + PlaneTypes[ppa->type].y > ppb->y && ppa->y < ppb->y + PlaneTypes[ppb->type].y;
}

static BOOL WINAPI CheckCollide(PPLANEINFO pp, PBULLETINFO pb) {
	return pb->x >= pp->x && pb->x < pp->x + PlaneTypes[pp->type].x &&
		pb->y >= pp->y && pb->y < pp->y + PlaneTypes[pp->type].y;
}

extern UINT GameState = 0;
extern UINT GameScore = 0;
extern BOOL KeyStates[5] = {};
static USHORT SummonCounter = 0;

static PLANEENTRY PlaneHead = {};
static BULLETENTRY BulletHead = {};

static VOID WINAPI ClearLists(VOID) {
	for (PPLANEENTRY pen = PlaneHead.next, nxt; pen != NULL; pen = nxt) {
		nxt = pen->next;
		MemoryFree(pen);
	}
	PlaneHead.next = NULL;
	for (PBULLETENTRY pen = BulletHead.next, nxt; pen != NULL; pen = nxt) {
		nxt = pen->next;
		MemoryFree(pen);
	}
	BulletHead.next = NULL;
}

static VOID WINAPI GeneratePlane(USHORT typ) {
	PPLANEENTRY pen = (PPLANEENTRY)MemoryAlloc(sizeof(PLANEENTRY));
	GeneratePlaneInfo(typ, &pen->plane);
	pen->prev = &PlaneHead;
	pen->next = PlaneHead.next;
	if (PlaneHead.next != NULL)
		PlaneHead.next->prev = pen;
	PlaneHead.next = pen;
}

static VOID WINAPI FireBullet(PPLANEENTRY ppen) {
	PBULLETENTRY pen = (PBULLETENTRY)MemoryAlloc(sizeof(BULLETENTRY));
	GenerateBulletInfo(&ppen->plane, ppen == &PlaneHead, &pen->bullet);
	pen->prev = &BulletHead;
	pen->next = BulletHead.next;
	if (BulletHead.next != NULL)
		BulletHead.next->prev = pen;
	BulletHead.next = pen;
}

static VOID WINAPI DestroyPlane(PPLANEENTRY pen) {
	PPLANEINFO pp = &pen->plane;
	if (pen != &PlaneHead) {
		GameScore += PlaneTypes[pp->type].health;
	}
	pp->type = 4;
	pp->dx = pp->dy = 0;
	pp->health = PlaneTypes[4].health;
}

static VOID WINAPI DeletePlane(PPLANEENTRY pen) {
	if (pen == &PlaneHead) {
		GameState = 0;
		return;
	}
	pen->prev->next = pen->next;
	if (pen->next != NULL)
		pen->next->prev = pen->prev;
	MemoryFree(pen);
}

static VOID WINAPI DeleteBullet(PBULLETENTRY pen) {
	pen->prev->next = pen->next;
	if (pen->next != NULL)
		pen->next->prev = pen->prev;
	MemoryFree(pen);
}

static VOID WINAPI HitPlane(PPLANEENTRY pena, PPLANEENTRY penb) {
	PPLANEINFO ppa = &pena->plane, ppb = &penb->plane;
	ppa->health -= max(PlaneTypes[ppb->type].dx, 10);
	ppb->health -= max(PlaneTypes[ppa->type].dx, 10);
	if ((SHORT)ppa->health <= 0 && ppa->type != 4)
		DestroyPlane(pena);
	if ((SHORT)ppb->health <= 0 && ppb->type != 4)
		DestroyPlane(penb);
}

static VOID WINAPI HitBullet(PPLANEENTRY ppen, PBULLETENTRY pben) {
	PPLANEINFO pp = &ppen->plane;
	pp->health -= pben->bullet.damage;
	if ((SHORT)pp->health <= 0 && pp->type != 4)
		DestroyPlane(ppen);
	pben->bullet.damage = 0;
}

VOID WINAPI StartGameplay(VOID) {
	GameState = 0;
	GameScore = 0;
	ClearLists();
	GeneratePlaneInfo(0, &PlaneHead.plane);
	GeneratePlane(1);
	UpdateGameplay();
	if (AudioHandle != NULL)
		PlayScore(AudioHandle, GenerateScore(Scores[1], 0, 0), 0xFFFF);
}

VOID WINAPI UpdateGameplay(VOID) {
	GameState++;
	PPLANEINFO pself = &PlaneHead.plane;
	if (KeyStates[0])
		pself->x -= 6;
	if (KeyStates[1])
		pself->x += 6;
	if (KeyStates[2])
		pself->y -= 4;
	if (KeyStates[3])
		pself->y += 4;
	static UINT ft;
	if (!KeyStates[4])
		ft = 0;
	if (KeyStates[4] && ft++ % 3 == 0) {
		FireBullet(&PlaneHead);
		PBULLETINFO pb = &BulletHead.next->bullet;
		pb->dx += (SHORT)(KeyStates[1] - KeyStates[0]);
		pb->dy += (SHORT)(-KeyStates[2] + KeyStates[3]);
		GameScore--;
	}
	SummonCounter++;
	USHORT thres = 40;
	if (GameState < 7200) {
		thres = (USHORT)(120 - GameState / 120);
	} else if (GameState < 14400) {
		thres = (USHORT)(60 + 20 - GameState / 360);
	}
	if (SummonCounter >= thres) {
		if (GameState >= 3600 && NextRandom(16) == 0) {
			GeneratePlane(3);
		} else if (GameState >= 1200 && NextRandom(4) == 0) {
			GeneratePlane(2);
		} else {
			GeneratePlane(1);
		}
		SummonCounter = 0;
	}
	for (PPLANEENTRY pen = &PlaneHead; pen != NULL; pen = pen->next) {
		PPLANEINFO pp = &pen->plane;
		if (pen != &PlaneHead && pp->type != 4 && NextRandom(60) == 0) {
			pp->dx += (NextRandom(8) + 1) / 2 - 2;
			if (pp->dx >= 4)
				pp->dx--;
			if (pp->dx <= -4)
				pp->dx++;
		}
		pp->x += pp->dx;
		pp->y += pp->dy;
		CheckEdge(pp);
		for (PPLANEENTRY penb = pen->next; penb != NULL; penb = penb->next) {
			if (CheckCollide(pp, &penb->plane))
				HitPlane(pen, penb);
		}
		if (pen != &PlaneHead && NextRandom(20) == 0)
			FireBullet(pen);
	}
	for (PBULLETENTRY pben = BulletHead.next; pben != NULL; pben = pben->next) {
		PBULLETINFO pb = &pben->bullet;
		pb->x += pb->dx;
		pb->y += pb->dy;
		for (PPLANEENTRY ppen = &PlaneHead; ppen != NULL; ppen = ppen->next) {
			if (CheckCollide(&ppen->plane, pb)) {
				HitBullet(ppen, pben);
				break;
			}
		}
		CheckEdge(pb);
	}
	for (PPLANEENTRY pen = &PlaneHead, nxt; pen != NULL; pen = nxt) {
		nxt = pen->next;
		PPLANEINFO pp = &pen->plane;
		pp->health += PlaneTypes[pp->type].dy;
		if ((SHORT)pp->health <= 0) {
			if (pen == &PlaneHead) {
				GameState = 0;
				return;
			}
			DeletePlane(pen);
			continue;
		} else if (pp->health > PlaneTypes[pp->type].health) {
			pp->health = PlaneTypes[pp->type].health;
		}
	}
	for (PBULLETENTRY pen = BulletHead.next, nxt; pen != NULL; pen = nxt) {
		nxt = pen->next;
		if ((SHORT)pen->bullet.damage <= 0)
			DeleteBullet(pen);
	}
	GameScore++;
}

VOID WINAPI PaintGameplay(HDC hdc) {
	HBITMAP hbuf = CreateCompatibleBitmap(hdc, GameplayWidth, GameplayHeight);
	HDC hbdc = CreateCompatibleDC(hdc);
	SelectObject(hbdc, hbuf);
	RECT rect = {0, 0, GameplayWidth, GameplayHeight};
	FillRect(hbdc, &rect, GetSysColorBrush(COLOR_WINDOW));
	for (PPLANEENTRY pen = &PlaneHead; pen != NULL; pen = pen->next) {
		PPLANEINFO pp = &pen->plane;
		DrawImage(hbdc, &Images[PlaneTypes[pp->type].type], pp->x, pp->y, 2);
	}
	for (PBULLETENTRY pen = BulletHead.next; pen != NULL; pen = pen->next) {
		PBULLETINFO pb = &pen->bullet;
		RECT rect = {pb->x - 1, pb->y - 1, pb->x + 1, pb->y + 2};
		FillRect(hbdc, &rect, GetSysColorBrush(COLOR_WINDOWTEXT));
	}
	rect.left = 24;
	rect.top = GameplayHeight - 16;
	rect.right = 24 + (GameplayWidth - 32) * PlaneHead.plane.health / PlaneTypes[PlaneHead.plane.type].health;
	rect.bottom = GameplayHeight - 8;
	DrawImage(hbdc, &Images[0], 8, GameplayHeight - 16, 0);
	FillRect(hbdc, &rect, GetSysColorBrush(COLOR_WINDOWTEXT));
	BitBlt(hdc, 0, 0, GameplayWidth, GameplayHeight, hbdc, 0, 0, SRCCOPY);
	DeleteDC(hbdc);
	DeleteObject(hbuf);
}
