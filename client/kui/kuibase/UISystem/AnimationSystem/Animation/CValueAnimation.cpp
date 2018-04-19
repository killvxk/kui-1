#include "OpenHMI.h"

namespace OpenHMI
{
	CValueAnimation::CValueAnimation()
	{
		;
	}

	CValueAnimation::~CValueAnimation()
	{
		;
	}

	void CValueAnimation::setParams( CBaseAmtData*			pAmtData, 
									LONG					lDurationTime, 
									CBaseAmtData*			pAmtStartValue, 
									CBaseAmtData*			pAmtEndValue,
									IAnimationListener*		pListener )
	{
		setAnimationData(pAmtData);
		setDurationTime(lDurationTime);
		setStartValue(pAmtStartValue);
		setEndValue(pAmtEndValue);
		addListener(pListener);
		
		if(m_pInterpolation != NULL)
		{
			m_pInterpolation->setInitialized(FALSE);
		}
	}
	
	void CValueAnimation::updateAnimationData(LONG lOffsetTime)
	{
		if (m_pAmtData == NULL || m_pInterpolation == NULL)
		{
			return;
		}
		
		if (!m_pInterpolation->isInitialized())
		{
			m_pInterpolation->setValue1(0, m_pAmtStartValue);
			m_pInterpolation->setValue2(getDurationTime(), m_pAmtEndValue);
			m_pInterpolation->setInitialized();
		}
		
		m_pAmtData->set(m_pInterpolation->getInterpolatedValue(lOffsetTime));
	}
}