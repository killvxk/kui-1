#include "OpenHMI.h"

namespace OpenHMI
{
	CAnimationGroup::CAnimationGroup()
	{
		;
	}

	CAnimationGroup::~CAnimationGroup()
	{
		;
	}

	INT CAnimationGroup::getAnimationCount()
	{
		return (INT)m_aryListAnimation.size();
	}

	CBaseAnimation* CAnimationGroup::getAnimation(UINT uiIndex)
	{
		return uiIndex >= m_aryListAnimation.size() ? NULL : m_aryListAnimation[uiIndex];
	}

	void CAnimationGroup::addAnimation(CBaseAnimation* pAnimation)
	{
		if (pAnimation == NULL)
		{
			return;
		}
		
		if (!m_aryListAnimation.isExist(pAnimation))
		{
			m_aryListAnimation.addItem(pAnimation);
		}
	}

	void CAnimationGroup::removeAnimation(CBaseAnimation* pAnimation)
	{
		if (pAnimation == NULL)
		{
			return;
		}
		
		if (m_aryListAnimation.isExist(pAnimation))
		{
			m_aryListAnimation.deleteItem(pAnimation);
		}
	}

	void CAnimationGroup::clearAllAnimations()
	{
		m_aryListAnimation.clear();
	}

	void CAnimationGroup::clearControlStatus()
	{
		CBaseAnimation::clearControlStatus();

		m_aryListAnimation.resetIterator();
		while (m_aryListAnimation.hasMore())
		{
			m_aryListAnimation.next()->clearControlStatus();
		}
	}

	LONG CAnimationGroup::getDurationTime()
	{
		LONG lDurationTime = 0;
		LONG lAnimationLength;
		
		for( UINT ui = 0; ui < m_aryListAnimation.size(); ui++ )
		{
			lAnimationLength = m_aryListAnimation[ui]->getAnimationLength();

			if (lAnimationLength > lDurationTime)
			{
				lDurationTime = lAnimationLength;
			}
		}
		
		return lDurationTime;
	}

	void CAnimationGroup::updateAnimationData(LONG lOffsetTime)
	{
		for( UINT ui = 0; ui < m_aryListAnimation.size(); ui++ )
		{
			m_aryListAnimation[ui]->runAnimation(lOffsetTime);
		}
	}	
}