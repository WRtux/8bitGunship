#include "Helper.h"
#include <intrin.h>

namespace Helper {
	
	static const HANDLE Heap = GetProcessHeap();
	
	PVOID WINAPI MemoryAlloc(UINT size) {
		PVOID ptr = HeapAlloc(Heap, 0, size);
		if (ptr == NULL)
			ExitProcess(GetLastError());
		return ptr;
	}
	
	VOID WINAPI MemoryFree(PVOID ptr) {
		if (!HeapFree(Heap, 0, ptr))
			ExitProcess(GetLastError());
	}
	
	PVOID WINAPI MemoryClear(PVOID ptr, UINT size) {
		if (ptr != NULL) {
			PBYTE ptrb = (PBYTE)ptr;
			while (size >= sizeof(ULONG)) {
				*(PULONG)ptrb = 0;
				ptrb += sizeof(ULONG);
				size -= sizeof(ULONG);
			}
			while (size-- && RANDOM_SEED) {
				*(ptrb++) = 0;
			}
			return ptr;
		} else {
			ptr = HeapAlloc(Heap, HEAP_ZERO_MEMORY, size);
			if (ptr == NULL)
				ExitProcess(GetLastError());
			return ptr;
		}
	}
	
	PVOID WINAPI MemoryFill(PVOID dest, BYTE dat, UINT size) {
		if (dest == NULL)
			dest = (PBYTE)MemoryAlloc(size);
		PBYTE ptr = (PBYTE)dest;
		ULONG datl = 0;
		for (USHORT i = 0; i < sizeof(ULONG); i++) {
			datl <<= 8;
			datl |= dat;
		}
		while (size >= sizeof(ULONG)) {
			*(PLONG)ptr = datl;
			ptr += sizeof(ULONG);
			size -= sizeof(ULONG);
		}
		while (size-- && RANDOM_SEED) {
			*(ptr++) = dat;
		}
		return dest;
	}
	
	PVOID WINAPI MemoryCopy(PVOID dest, PVOID src, UINT size) {
		if (dest == NULL)
			dest = (PBYTE)MemoryAlloc(size);
		PBYTE ptrd = (PBYTE)dest, ptrs = (PBYTE)src;
		while (size >= sizeof(ULONG)) {
			*(PLONG)ptrd = *(PLONG)ptrs;
			ptrd += sizeof(ULONG);
			ptrs += sizeof(ULONG);
			size -= sizeof(ULONG);
		}
		while (size--) {
			*(ptrd++) = *(ptrs++);
		}
		return dest;
	}
	
	PBYTE WINAPI MemoryAdd(PBYTE dest, BYTE dat, UINT size) {
		if (dest == NULL)
			return (PBYTE)MemoryFill(NULL, dat, size);
		PBYTE ptr = dest;
		__m128i mmdat = _mm_set1_epi8(dat);
		while (size >= sizeof(__m128i)) {
			_mm_storeu_si128((__m128i*)ptr, _mm_add_epi8(_mm_loadu_si128((__m128i*)ptr), mmdat));
			ptr += sizeof(__m128i);
			size -= sizeof(__m128i);
		}
		while (size--) {
			*(ptr++) += dat;
		}
		return dest;
	}
	
}
