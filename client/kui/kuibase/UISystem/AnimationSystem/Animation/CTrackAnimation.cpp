#include "OpenHMI.h"

namespace OpenHMI
{
	CTrackAnimation::CTrackAnimation()
	{
		m_pAmtStartPercent = new CAmtFloat(0.0f);
		m_pAmtEndPercent = new CAmtFloat(1.0f);
	}

	CTrackAnimation::~CTrackAnimation()
	{
		;
	}

	void CTrackAnimation::setAnimationData(CAmtPoint* pAmtpData)
	{
		CBaseAnimation::setAnimationData(pAmtpData);
	}

	void CTrackAnimation::setPath(CBasePath* pPath)
	{
		m_pPath = pPath;
	}

	void CTrackAnimation::setStartPercent(FLOAT fValue)
	{
		m_pAmtStartPercent->m_fValue = fValue;
	}

	void CTrackAnimation::setEndPercent(FLOAT fValue)
	{
		m_pAmtEndPercent->m_fValue = fValue;
	}

	void CTrackAnimation::setParams( 
					CAmtPoint*				pAmtPoint,  
					LONG					lDurationTime,
					CBasePath*				pPath,
					FLOAT					fStartPercent, 
					FLOAT					fEndPercent,
					IAnimationListener*		pListener)
	{
		setAnimationData(pAmtPoint);
		setDurationTime(lDurationTime);
		setPath(pPath);
		setStartPercent(fStartPercent);
		setEndPercent(fEndPercent);
		addListener(pListener);

		if(m_pInterpolation != NULL)
		{
			m_pInterpolation->setInitialized(FALSE);
		}
	}
	
	void CTrackAnimation::updateAnimationData(LONG lOffsetTime)
	{
		if (m_pAmtData == NULL || m_pInterpolation == NULL)
		{
			return;
		}
		
		if (!m_pInterpolation->isInitialized())
		{
			m_pInterpolation->setValue1(0, m_pAmtStartPercent);
			m_pInterpolation->setValue2(getDurationTime(), m_pAmtEndPercent);
			m_pInterpolation->setInitialized();
		}
		
		CAmtFloat* pAmtfValue = (CAmtFloat*)m_pInterpolation->getInterpolatedValue(lOffsetTime);
				
		m_pAmtData->set(m_pPath->getPointByPercent(pAmtfValue->m_fValue));
	}
}