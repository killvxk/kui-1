#include "OpenHMI.h"

namespace OpenHMI
{
	void Math::swapAtMinMax(INT &iA, INT &iB)
	{
		if (iA > iB)
		{
			INT iTemp = iA;
			iA = iB;
			iB = iTemp;
		}
	}

	void Math::swapAtMinMax(UINT &uiA, UINT &uiB)
	{
		if (uiA > uiB)
		{
			UINT uiTemp = uiA;
			uiA = uiB;
			uiB = uiTemp;
		}
	}

	void Math::swapAtMinMax(FLOAT &fA, FLOAT &fB)
	{
		if (fA > fB)
		{
			FLOAT fTemp = fA;
			fA = fB;
			fB = fTemp;
		}
	}

	void Math::swapAtMinMax(DOUBLE &dA, DOUBLE &dB)
	{
		if (dA > dB)
		{
			DOUBLE dTemp = dA;
			dA = dB;
			dB = dTemp;
		}
	}

	INT Math::pow(INT iValue, UINT uiCount)
	{
		INT iRet = 1;
		for (UINT i = 0; i < uiCount; i++)
		{
			iRet *= iValue;
		}

		return iRet;
	}

	FLOAT Math::pow(FLOAT fValue, UINT uiCount)
	{
		FLOAT fRet = 1;
		for (UINT i = 0; i < uiCount; i++)
		{
			fRet *= fValue;
		}

		return fRet;
	}

	DOUBLE Math::pow(DOUBLE dValue, UINT uiCount)
	{
		DOUBLE dRet = 1;
		for (UINT i = 0; i < uiCount; i++)
		{
			dRet *= dValue;
		}

		return dRet;
	}

	DOUBLE Math::sqrt(DOUBLE dValue)
	{
		DOUBLE dEpsilonNum,dGuessNum;
		dEpsilonNum = 1.0E-5;
		dGuessNum = 1.0f;

		while( abs(dGuessNum * dGuessNum - abs(dValue)) >= dEpsilonNum )
		{
			dGuessNum = (DOUBLE)((abs(dValue) / dGuessNum + dGuessNum) / 2.0);
		}

		return dGuessNum;
	}
}