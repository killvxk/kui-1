#pragma once

namespace OpenHMI
{
	typedef UINT (*BilinearFunc)(const UINT, const UINT, const UINT, const UINT);

	UINT gBilerp4_00(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_01(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_02(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_03(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_10(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_11(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_12(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_13(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_20(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_21(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_22(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_23(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_30(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_31(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_32(UINT c00, UINT c01, UINT c10, UINT c11);
	UINT gBilerp4_33(UINT c00, UINT c01, UINT c10, UINT c11);

	extern const BilinearFunc g_aBilinearFuncs[4 * 4];
}