#include "OpenHMI.h"

namespace OpenHMI
{
	CKeyFrameAnimation::CKeyFrameAnimation()
	{
		;
	}

	CKeyFrameAnimation::~CKeyFrameAnimation()
	{
		;
	}

	UINT CKeyFrameAnimation::getKeyFrameCount()
	{
		return (UINT)m_aryKeyFrame.size();
	}

	CKeyFrame* CKeyFrameAnimation::getKeyFrame(UINT uiIndex)
	{		
		return uiIndex >= m_aryKeyFrame.size() ? NULL : m_aryKeyFrame[uiIndex];
	}

	void CKeyFrameAnimation::addKeyFrame(CKeyFrame* pFrame)
	{
		if( !m_aryKeyFrame.isExist(pFrame) )
		{
			m_aryKeyFrame.addItem(pFrame);
		}
	}

	void CKeyFrameAnimation::removeKeyFrame(UINT uiIndex)
	{
		m_aryKeyFrame.deleteItemByIndex(uiIndex);
	}

	void CKeyFrameAnimation::clearAllKeyFrames()
	{
		m_aryKeyFrame.clear();
	}

	void CKeyFrameAnimation::setParams( CBaseAmtData* pAmtData,IAnimationListener* pListener )
	{
		setAnimationData(pAmtData);
		addListener(pListener);
		
		if(m_pInterpolation != NULL)
		{
			m_pInterpolation->setInitialized(FALSE);
		}
	}
	
	void CKeyFrameAnimation::updateAnimationData(LONG lOffsetTime)
	{
		if (m_pAmtData == NULL || m_pInterpolation == NULL)
		{
			return;
		}
		
		if (m_aryKeyFrame.size() == 0)
		{
			return;
		}
		
		UINT uiPos = 0;
		
		for ( ; uiPos < m_aryKeyFrame.size(); uiPos++)
		{
			if (lOffsetTime < ((CKeyFrame*)m_aryKeyFrame[uiPos])->getTime())
			{
				break;
			}
		}
		
		CKeyFrame* pKeyFrame1;
		CKeyFrame* pKeyFrame2;
		
		if (uiPos == 0)
		{
			return;
		}
		else if (uiPos == m_aryKeyFrame.size())
		{
			pKeyFrame1 = (CKeyFrame*)m_aryKeyFrame[m_aryKeyFrame.size() - 1];
			pKeyFrame2 = pKeyFrame1;
		}
		else
		{
			pKeyFrame1 = (CKeyFrame*)m_aryKeyFrame[uiPos - 1];
			pKeyFrame2 = (CKeyFrame*)m_aryKeyFrame[uiPos];
		}
		
		m_pInterpolation->setValue1(pKeyFrame1->getTime(),pKeyFrame1->getValue());
		m_pInterpolation->setValue2(pKeyFrame2->getTime(),pKeyFrame2->getValue());
		
		m_pAmtData->set(m_pInterpolation->getInterpolatedValue(lOffsetTime));
	}
	
	LONG CKeyFrameAnimation::getDurationTime()
	{
		if (m_aryKeyFrame.size() == 0)
		{
			return 0;
		}
		else
		{
			return ((CKeyFrame*)m_aryKeyFrame[m_aryKeyFrame.size() - 1])->getTime();
		}
	}
}