#include "../Helper/Helper.h"
#include "Resources.h"
using namespace Helper;

#define VOCAB_COUNT 9

extern PTSTR Vocabulary[VOCAB_COUNT] = {};

VOID WINAPI BuildVocabulary(PTSTR dat) {
	PTSTR st = dat;
	for (USHORT i = 0; i < VOCAB_COUNT; dat++) {
		if (*dat == L'\xFFFF') {
			*dat = L'\0';
			Vocabulary[i++] = st;
			st = dat + 1;
		}
	}
}
