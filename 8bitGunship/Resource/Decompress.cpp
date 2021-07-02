#include "../Helper/Helper.h"
using namespace Helper;

typedef union _HUFFNODE {
	_HUFFNODE *nodes[2];
	TCHAR character;
} HUFFNODE, *PHUFFNODE;

static PTSTR WINAPI BuildHuffmanNode(PTSTR tree, PHUFFNODE trg) {
	PHUFFNODE pnode = (PHUFFNODE)MemoryAlloc(sizeof(HUFFNODE) * 2);
	PTSTR nxt = &tree[2];
	trg->nodes[0] = &pnode[0];
	trg->nodes[1] = &pnode[1];
	if (tree[0] == L'\0') {
		nxt = BuildHuffmanNode(nxt, &pnode[0]);
	} else {
		pnode[0].nodes[1] = NULL;
		pnode[0].character = tree[0];
	}
	if (tree[1] == L'\0') {
		nxt = BuildHuffmanNode(nxt, &pnode[1]);
	} else {
		pnode[1].nodes[1] = NULL;
		pnode[1].character = tree[1];
	}
	return nxt;
}

static VOID WINAPI FreeHuffmanNode(PHUFFNODE node) {
	if (node->nodes[1] != NULL)
		FreeHuffmanNode(node->nodes[0]);
	MemoryFree(node);
}

PTSTR WINAPI DecompressHuffman(PVOID dat, PTSTR tree, PTSTR buf) {
	PBYTE ptrd = (PBYTE)dat;
	UINT size = *(PUINT)ptrd;
	ptrd += sizeof(UINT);
	if (buf == NULL)
		buf = (PTSTR)MemoryAlloc(sizeof(TCHAR) * size);
	PHUFFNODE phead = (PHUFFNODE)MemoryAlloc(sizeof(HUFFNODE));
	BuildHuffmanNode(tree, phead);
	UINT dbpos = 0;
	for (UINT cnt = 0; cnt < size; cnt++) {
		PHUFFNODE pos = phead;
		while (pos->nodes[1] != NULL) {
			pos = pos->nodes[ptrd[dbpos / 8] & (1 << dbpos % 8) ? 1 : 0];
			dbpos++;
		}
		buf[cnt] = pos->character;
	}
	FreeHuffmanNode(phead);
	return buf;
}

PVOID WINAPI DecompressLZSS(PVOID dat, PVOID buf) {
	PBYTE ptrd = (PBYTE)dat, ptrb;
	UINT size = *(PUINT)ptrd, cnt = 0;
	if (buf == NULL)
		buf = (PBYTE)MemoryAlloc(size);
	ptrb = (PBYTE)buf;
	ptrd += sizeof(UINT);
	while(cnt < size) { 
		BYTE sgns = *(ptrd++);
		for (USHORT i = 0; i < 8 && cnt < size; i++) {
			if (sgns & 1 << i) {
				USHORT off = *(PUSHORT)ptrd, len;
				len = (off >> 10) + 3;
				off &= (1 << 10) - 1;
				ptrd += sizeof(USHORT);
				for (USHORT j = 0; j < len; j++) {
					*(ptrb++) = *(ptrb - (1 << 10) + off);
				}
				cnt += len;
			} else {
				*(ptrb++) = *(ptrd++);
				cnt++;
			}
		}
	}
	return buf;
}
