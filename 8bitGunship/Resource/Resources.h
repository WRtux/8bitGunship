#pragma once

#include <minwindef.h>
#include "ResourceID.h"

extern PTSTR WINAPI DecompressHuffman(PVOID, PTSTR, PTSTR);
extern PVOID WINAPI DecompressLZSS(PVOID, PVOID);

#define LoadHTree(id) (PTSTR)LockResource(LoadResource(NULL, FindResourceA(NULL, MAKEINTRESOURCEA(id), "HTREE")))
#define LoadDecompressHuffman(id, tree, buf) DecompressHuffman(LockResource(LoadResource(NULL, FindResourceA(NULL, MAKEINTRESOURCEA(id), "HUFFMAN"))), (tree), (buf))
#define LoadDecompressLZSS(id, buf) DecompressLZSS(LockResource(LoadResource(NULL, FindResourceA(NULL, MAKEINTRESOURCEA(id), "LZSS"))), (buf))

#define VOCAB_MAIN_TITLE 0
#define VOCAB_MAIN_START 1
#define VOCAB_MAIN_ABOUT 2
#define VOCAB_MAIN_EXIT 3
#define VOCAB_INFO_ABOUT 4
#define VOCAB_INFO_HELP_TITLE 5
#define VOCAB_INFO_HELP 6
#define VOCAB_INFO_GAMEOVER_TITLE 7
#define VOCAB_INFO_GAMEOVER 8

extern PTSTR Vocabulary[];

extern VOID WINAPI BuildVocabulary(PTSTR);
