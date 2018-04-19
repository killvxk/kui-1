#include "OpenHMI.h"

namespace OpenHMI
{
	CDiscreteInterpolation::CDiscreteInterpolation()
	{
		;
	}

	CDiscreteInterpolation::~CDiscreteInterpolation()
	{
		;
	}

	CBaseAmtData* CDiscreteInterpolation::getInterpolatedValue(LONG lTime)
	{
		if (m_pAmtValue1 == NULL || m_pAmtValue2 == NULL)
		{
			return NULL;
		}
		
		if (lTime <= m_lTime2)
		{
			return m_pAmtValue1->cloneData();
		}
		else
		{
			return m_pAmtValue2->cloneData();
		}
	}
}