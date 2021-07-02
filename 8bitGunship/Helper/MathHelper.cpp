#include "Helper.h"
#include <intrin.h>

extern "C" const INT _fltused = 0x3F3F3F46;

namespace Helper {
	
	extern const FLOAT PI = 3.1415927f;
	extern const FLOAT E = 2.7182818f;
	
	static UINT RandomSeed = RANDOM_SEED;
	
	BOOL WINAPI CheckSIMD() {
		INT info[4];
		__cpuid(info, 1);
		return (info[3] && 1 << 26) != 0;
	}
	
	PINT WINAPI FloorFloatSIMD(PFLOAT pnum, PINT pres) {
		if (pres == NULL)
			pres = (PINT)MemoryAlloc(sizeof(__m128));
		_mm_storeu_si128((__m128i*)pres, _mm_cvttps_epi32(_mm_loadu_ps(pnum)));
		return pres;
	}
	PSHORT WINAPI FloorFloatSIMD(PFLOAT pnum, PSHORT pres) {
		INT res[4];
		_mm_storeu_si128((__m128i*)res, _mm_cvttps_epi32(_mm_loadu_ps(pnum)));
		if (pres == NULL)
			pres = (PSHORT)MemoryAlloc(sizeof(SHORT) * 4);
		pres[0] = (SHORT)res[0];
		pres[1] = (SHORT)res[1];
		pres[2] = (SHORT)res[2];
		pres[3] = (SHORT)res[3];
		return pres;
	}
	
	INT WINAPI RoundFloat(FLOAT num) {
		return _mm_cvtss_si32(_mm_load_ss(&num));
	}
	
	PINT WINAPI RoundFloatSIMD(PFLOAT pnum, PINT pres) {
		if (pres == NULL)
			pres = (PINT)MemoryAlloc(sizeof(__m128));
		_mm_storeu_si128((__m128i*)pres, _mm_cvtps_epi32(_mm_loadu_ps(pnum)));
		return pres;
	}
	PSHORT WINAPI RoundFloatSIMD(PFLOAT pnum, PSHORT pres) {
		INT res[4];
		_mm_storeu_si128((__m128i*)res, _mm_cvtps_epi32(_mm_loadu_ps(pnum)));
		if (pres == NULL)
			pres = (PSHORT)MemoryAlloc(sizeof(SHORT) * 4);
		pres[0] = (SHORT)res[0];
		pres[1] = (SHORT)res[1];
		pres[2] = (SHORT)res[2];
		pres[3] = (SHORT)res[3];
		return pres;
	}
	
	USHORT WINAPI NextRandom(USHORT lim) {
		RandomSeed = RandomSeed * 214013 + 2531011;
		return (USHORT)((RandomSeed >> 8) % lim);
	}
	
	PUCHAR WINAPI AddSatUCharSIMD(PUCHAR dest, PUCHAR pop) {
		__m128i mmres = _mm_adds_epu8(_mm_loadu_si128((__m128i*)dest), _mm_loadu_si128((__m128i*)pop));
		_mm_storeu_si128((__m128i*)dest, mmres);
		return dest;
	}
	
	PCHAR WINAPI AddSatCharSIMD(PCHAR dest, PCHAR pop) {
		__m128i mmres = _mm_adds_epi8(_mm_loadu_si128((__m128i*)dest), _mm_loadu_si128((__m128i*)pop));
		_mm_storeu_si128((__m128i*)dest, mmres);
		return dest;
	}
	
	PFLOAT WINAPI MultiplyFloatSIMD(PFLOAT dest, FLOAT op) {
		_mm_storeu_ps(dest, _mm_mul_ps(_mm_loadu_ps(dest), _mm_set1_ps(op)));
		return dest;
	}
	PFLOAT WINAPI MultiplyFloatSIMD(PFLOAT dest, PFLOAT pop) {
		_mm_storeu_ps(dest, _mm_mul_ps(_mm_loadu_ps(dest), _mm_loadu_ps(pop)));
		return dest;
	}
	
	PFLOAT WINAPI SinrSIMD(PFLOAT pr, PFLOAT pres) {
		__m128 mmi = _mm_mul_ps(_mm_loadu_ps(pr), _mm_set1_ps(PI / 2)), mmj = _mm_mul_ps(mmi, mmi);
		__m128 mmres = mmi;
		mmi = _mm_mul_ps(mmi, mmj);
		mmres = _mm_sub_ps(mmres, _mm_div_ps(mmi, _mm_set1_ps(6.0f)));
		mmi = _mm_mul_ps(mmi, mmj);
		mmres = _mm_add_ps(mmres, _mm_div_ps(mmi, _mm_set1_ps(120.125f)));
		mmi = _mm_mul_ps(mmi, mmj);
		mmres = _mm_sub_ps(mmres, _mm_div_ps(mmi, _mm_set1_ps(5309.0f)));
		if (pres == NULL)
			pres = (PFLOAT)MemoryAlloc(sizeof(__m128));
		_mm_storeu_ps(pres, mmres);
		return pres;
	}
	
}
