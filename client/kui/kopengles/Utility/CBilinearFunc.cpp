#include "RenderEngine.h"

namespace OpenHMI
{
	#define MASK            0xFF00FF
	#define LO_PAIR(x)      ((x) & MASK)
	#define HI_PAIR(x)      (((x) >> 8) & MASK)
	#define COMBINE(lo, hi) (((lo) & ~0xFF00) | (((hi) & ~0xFF00) << 8))

	const BilinearFunc g_aBilinearFuncs[4 * 4] =
	{
		gBilerp4_00, gBilerp4_01, gBilerp4_02, gBilerp4_03,
		gBilerp4_10, gBilerp4_11, gBilerp4_12, gBilerp4_13,
		gBilerp4_20, gBilerp4_21, gBilerp4_22, gBilerp4_23,
		gBilerp4_30, gBilerp4_31, gBilerp4_32, gBilerp4_33
	};
	
	UINT gBilerp4_00(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		return c00;
	}

	UINT gBilerp4_01(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (3 * LO_PAIR(c00) + LO_PAIR(c01)) >> 2;
		UINT hi = (3 * HI_PAIR(c00) + HI_PAIR(c01)) >> 2;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_02(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (LO_PAIR(c00) + LO_PAIR(c01)) >> 1;
		UINT hi = (HI_PAIR(c00) + HI_PAIR(c01)) >> 1;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_03(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (LO_PAIR(c00) + 3 * LO_PAIR(c01)) >> 2;
		UINT hi = (HI_PAIR(c00) + 3 * HI_PAIR(c01)) >> 2;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_10(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (3 * LO_PAIR(c00) + LO_PAIR(c10)) >> 2;
		UINT hi = (3 * HI_PAIR(c00) + HI_PAIR(c10)) >> 2;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_11(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (9 * LO_PAIR(c00) + 3 * (LO_PAIR(c01) + LO_PAIR(c10)) + LO_PAIR(c11)) >> 4;
		UINT hi = (9 * HI_PAIR(c00) + 3 * (HI_PAIR(c01) + HI_PAIR(c10)) + HI_PAIR(c11)) >> 4;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_12(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (3 * (LO_PAIR(c00) + LO_PAIR(c01)) + LO_PAIR(c10) + LO_PAIR(c11)) >> 3;
		UINT hi = (3 * (HI_PAIR(c00) + HI_PAIR(c01)) + HI_PAIR(c10) + HI_PAIR(c11)) >> 3;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_13(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (9 * LO_PAIR(c01) + 3 * (LO_PAIR(c00) + LO_PAIR(c11)) + LO_PAIR(c10)) >> 4;
		UINT hi = (9 * HI_PAIR(c01) + 3 * (HI_PAIR(c00) + HI_PAIR(c11)) + HI_PAIR(c10)) >> 4;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_20(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (LO_PAIR(c00) + LO_PAIR(c10)) >> 1;
		UINT hi = (HI_PAIR(c00) + HI_PAIR(c10)) >> 1;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_21(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (3 * (LO_PAIR(c00) + LO_PAIR(c10)) + LO_PAIR(c01) + LO_PAIR(c11)) >> 3;
		UINT hi = (3 * (HI_PAIR(c00) + HI_PAIR(c10)) + HI_PAIR(c01) + HI_PAIR(c11)) >> 3;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_22(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (LO_PAIR(c00) + LO_PAIR(c01) + LO_PAIR(c10) + LO_PAIR(c11)) >> 2;
		UINT hi = (HI_PAIR(c00) + HI_PAIR(c01) + HI_PAIR(c10) + HI_PAIR(c11)) >> 2;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_23(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (3 * (LO_PAIR(c01) + LO_PAIR(c11)) + LO_PAIR(c00) + LO_PAIR(c10)) >> 3;
		UINT hi = (3 * (HI_PAIR(c01) + HI_PAIR(c11)) + HI_PAIR(c00) + HI_PAIR(c10)) >> 3;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_30(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (LO_PAIR(c00) + 3 * LO_PAIR(c10)) >> 2;
		UINT hi = (HI_PAIR(c00) + 3 * HI_PAIR(c10)) >> 2;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_31(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (9 * LO_PAIR(c10) + 3 * (LO_PAIR(c00) + LO_PAIR(c11)) + LO_PAIR(c01)) >> 4;
		UINT hi = (9 * HI_PAIR(c10) + 3 * (HI_PAIR(c00) + HI_PAIR(c11)) + HI_PAIR(c01)) >> 4;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_32(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (3 * (LO_PAIR(c10) + LO_PAIR(c11)) + LO_PAIR(c00) + LO_PAIR(c01)) >> 3;
		UINT hi = (3 * (HI_PAIR(c10) + HI_PAIR(c11)) + HI_PAIR(c00) + HI_PAIR(c01)) >> 3;
		return COMBINE(lo, hi);
	}

	UINT gBilerp4_33(UINT c00, UINT c01, UINT c10, UINT c11)
	{
		UINT lo = (9 * LO_PAIR(c11) + 3 * (LO_PAIR(c01) + LO_PAIR(c10)) + LO_PAIR(c00)) >> 4;
		UINT hi = (9 * HI_PAIR(c11) + 3 * (HI_PAIR(c01) + HI_PAIR(c10)) + HI_PAIR(c00)) >> 4;
		return COMBINE(lo, hi);
	}
}