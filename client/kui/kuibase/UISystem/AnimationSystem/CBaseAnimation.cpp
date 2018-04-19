#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseAnimation::CBaseAnimation()
		: m_pAmtData(NULL)
		, m_pInterpolation(new CLinearInterpolation())
		, m_lDelayTime(0)
		, m_lDurationTime(0)
		, m_pSequentAnimation(NULL)
		, m_bHasStarted(FALSE)
		, m_bHasEnded(FALSE)
	{
		;
	}

	CBaseAnimation::~CBaseAnimation()
	{
		;
	}

	void CBaseAnimation::setAnimationData(CBaseAmtData* pAmtData)
	{
		m_pAmtData = pAmtData;
	}

	void CBaseAnimation::setInterpolation(CBaseInterpolation* pInterpolation)
	{
		m_pInterpolation = pInterpolation;
	}

	void CBaseAnimation::setDelayTime(LONG lValue)
	{
		m_lDelayTime = lValue < 0 ? 0 : lValue;
	}

	void CBaseAnimation::setDurationTime(LONG lValue)
	{
		m_lDurationTime = lValue < 0 ? 0 : lValue;
	}	

	void CBaseAnimation::setSequentAnimation(CBaseAnimation* pAnimation)
	{
		m_pSequentAnimation = pAnimation;
	}

	void CBaseAnimation::clearControlStatus()
	{
		m_bHasStarted = FALSE;
		m_bHasEnded = FALSE;
		
		if (m_pSequentAnimation != NULL)
		{
			m_pSequentAnimation->clearControlStatus();
		}
	}

	LONG CBaseAnimation::getAnimationLength()
	{
		LONG lTimeLength = m_lDelayTime + getDurationTime();
		
		if (m_pSequentAnimation != NULL)
		{
			lTimeLength += m_pSequentAnimation->getAnimationLength();
		}
		
		return lTimeLength;
	}

	void CBaseAnimation::runAnimation(LONG lCurrentTime)
	{
		LONG lOffsetTime = lCurrentTime - m_lDelayTime;
		LONG lDurationTime = getDurationTime();
		
		if (lOffsetTime < 0)
		{
			return;
		}
		else if (lOffsetTime >= lDurationTime)
		{
			if (!m_bHasEnded)
			{
				updateAnimationData(lDurationTime);
				fireValueChanged();
				
				m_bHasEnded = true;
				fireEnded();
			}
			
			if (m_pSequentAnimation != NULL)
			{
				m_pSequentAnimation->runAnimation(lOffsetTime - lDurationTime);
			}
		}
		else
		{
			if (!m_bHasStarted)
			{
				m_bHasStarted = true;
				fireStarted();
			}
			
			updateAnimationData(lOffsetTime);			
			fireValueChanged();
		}
	}

	void CBaseAnimation::addListener(IAnimationListener* pListener)
	{
		if (pListener == NULL)
		{
			return;
		}

		if (!m_aryListener.isExist(pListener))
		{
			m_aryListener.addItem(pListener);
		}
	}

	void CBaseAnimation::removeListener(IAnimationListener* pListener)
	{
		if (pListener == NULL)
		{
			return;
		}

		if (m_aryListener.isExist(pListener))
		{
			m_aryListener.deleteItem(pListener);
		}
	}

	LONG CBaseAnimation::getDurationTime()
	{
		return m_lDurationTime;
	}
	
	void CBaseAnimation::fireStarted()
	{
		m_aryListener.resetIterator();
		while (m_aryListener.hasMore())
		{
			m_aryListener.next()->onAnimationStarted(this);
		}
	}

	void CBaseAnimation::fireValueChanged()
	{
		m_aryListener.resetIterator();
		while (m_aryListener.hasMore())
		{
			m_aryListener.next()->onAnimationValueChanged(this);
		}
	}

	void CBaseAnimation::fireEnded()
	{
		m_aryListener.resetIterator();
		while (m_aryListener.hasMore())
		{
			m_aryListener.next()->onAnimationEnded(this);
		}
	}
}