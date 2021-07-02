#include "Helper.h"

namespace Helper {
	
	#define BUFFER_LENGTH 0x100
	
	#define DECIMAL_LENGTH 10
	#define HEXADECIMAL_LENGTH 8
	
	static TCHAR buffer[BUFFER_LENGTH];
	
	PTSTR WINAPI StringAlloc(USHORT len) {
		return (PTSTR)MemoryClear(NULL, sizeof(TCHAR) * (len + 1));
	}
	
	USHORT WINAPI StringCopy(PTSTR dest, PTSTR src, USHORT lim) {
		USHORT len = 0;
		while (lim-- != 0) {
			TCHAR ch = src[len];
			if (ch == L'\0')
				break;
			dest[len] = ch;
			len++;
		}
		dest[len] = L'\0';
		return len;
	}
	
	USHORT WINAPI StringAppend(PTSTR dest, PTSTR str, USHORT lim) {
		USHORT dlen = 0, len = 0;
		while (*dest != L'\0') {
			dest++;
			dlen++;
		}
		while (lim-- != 0) {
			TCHAR ch = str[len];
			if (ch == L'\0')
				break;
			dest[len] = ch;
			len++;
		}
		dest[len] = L'\0';
		return dlen + len;
	}
	
	USHORT WINAPI SimpleDecimal(PTSTR str, INT num) {
		BOOL neg = num < 0;
		UINT abs = neg ? -num : num;
		USHORT i = BUFFER_LENGTH - 1;
		buffer[i] = L'\0';
		do {
			buffer[--i] = L'0' + abs % 10;
			abs /= 10;
		} while (abs != 0);
		if (neg)
			buffer[--i] = L'-';
		USHORT len = BUFFER_LENGTH - i;
		MemoryCopy(str, buffer + i, sizeof(TCHAR) * len);
		return len - 1;
	}
	
	USHORT WINAPI SimpleHexadecimal(PTSTR str, UINT num) {
		USHORT i = HEXADECIMAL_LENGTH;
		str[i] = L'\0';
		do {
			CHAR j = num & 0xF;
			str[--i] = (j < 10) ? L'0' + j : L'A' + j - 10;
			num >>= 4;
		} while (i != 0);
		return HEXADECIMAL_LENGTH;
	}
	
	USHORT WINAPI FormatDecimal(PTSTR str, UINT num, TCHAR pad) {
		if (pad == L'\0')
			return SimpleDecimal(str, num);
		USHORT i = DECIMAL_LENGTH;
		str[i] = L'\0';
		do {
			str[--i] = L'0' + num % 10;
			num /= 10;
		} while (num != 0);
		while (i != 0) {
			str[--i] = pad;
		}
		return DECIMAL_LENGTH;
	}
	
}
