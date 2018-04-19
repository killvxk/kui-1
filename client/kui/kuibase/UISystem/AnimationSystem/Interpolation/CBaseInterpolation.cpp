#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseInterpolation::CBaseInterpolation()
		: m_lTime1(0)
		, m_lTime2(0)
		, m_bInitialized(FALSE)
	{
		;
	}

	CBaseInterpolation::~CBaseInterpolation()
	{
		;
	}

	void CBaseInterpolation::setValue1(LONG lTime, CBaseAmtData* pAmtValue)
	{
		m_lTime1 = lTime;
		m_pAmtValue1 = pAmtValue;
	}

	void CBaseInterpolation::setValue2(LONG lTime, CBaseAmtData* pAmtValue)
	{
		m_lTime2 = lTime;
		m_pAmtValue2 = pAmtValue;
	}

	BOOLEAN CBaseInterpolation::isInitialized()
	{
		return m_bInitialized;
	}

	void CBaseInterpolation::setInitialized(BOOLEAN bValue)
	{
		m_bInitialized = bValue;
	}
}