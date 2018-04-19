#include "OpenHMI.h"

namespace OpenHMI
{
	CUniformRectilinearMotionAnimation::CUniformRectilinearMotionAnimation()
	{
		;
	}

	CUniformRectilinearMotionAnimation::~CUniformRectilinearMotionAnimation()
	{
		;
	}

	void CUniformRectilinearMotionAnimation::setStartPoint(CAmtVector2* pAmtvValue)
	{
		m_pAmtvStartPoint = pAmtvValue;
	}
	
	void CUniformRectilinearMotionAnimation::setSpeed(CAmtVector2* pAmtvValue)
	{
		m_pAmtvSpeed = pAmtvValue;
	}
	
	void CUniformRectilinearMotionAnimation::setParams(	CAmtPoint*		pAmtpData, 
														LONG			lDurationTime, 
														CAmtVector2*	pAmtvStartPoint, 
														CAmtVector2*	pAmtvSpeed,
														IAnimationListener*	pListener)
	{
		setAnimationData(pAmtpData);
		setDurationTime(lDurationTime);
		setStartPoint(pAmtvStartPoint);
		setSpeed(pAmtvSpeed);
		addListener(pListener);

		if(m_pInterpolation != NULL)
		{
			m_pInterpolation->setInitialized(FALSE);
		}
	}
	
	void CUniformRectilinearMotionAnimation::updateAnimationData(LONG lOffsetTime)
	{
		if (m_pAmtData == NULL)
		{
			return;
		}
		
		CAmtVector2* pAmtvPoint = (CAmtVector2*)m_pAmtvStartPoint->add(m_pAmtvSpeed->multiply((FLOAT)lOffsetTime));
		
		((CAmtPoint*)((CBaseAmtData*)m_pAmtData))->m_iX = (INT)pAmtvPoint->m_fX;
		((CAmtPoint*)((CBaseAmtData*)m_pAmtData))->m_iY = (INT)pAmtvPoint->m_fY;
	}
}