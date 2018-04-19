#include "OpenHMI.h"

namespace OpenHMI
{
	CLinearInterpolation::CLinearInterpolation()
	{
		;
	}

	CLinearInterpolation::~CLinearInterpolation()
	{
		;
	}

	CBaseAmtData* CLinearInterpolation::getInterpolatedValue(LONG lTime)
	{
		if (m_pAmtValue1 == NULL || m_pAmtValue2 == NULL)
		{
			return NULL;
		}
		
		if (lTime <= m_lTime1)
		{
			return m_pAmtValue1->cloneData();
		}
		else if (lTime >= m_lTime2)
		{
			return m_pAmtValue2->cloneData();
		}
		else
		{
			return m_pAmtValue1->add(m_pAmtValue2->subtract(m_pAmtValue1)->multiply((FLOAT)(lTime  - m_lTime1) / (m_lTime2 - m_lTime1)));
		}
	}
}