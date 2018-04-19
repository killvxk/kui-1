#include "OpenHMI.h"

namespace OpenHMI
{
	CTableAmtTouchShake::CTableAmtTouchShake()
		: m_pComponent(NULL)
	{
		m_pAnimation = new CKeyFrameAnimation();
	}

	CTableAmtTouchShake::~CTableAmtTouchShake()
	{
		;
	}

	BOOLEAN CTableAmtTouchShake::prepareAmtData(Object* pData)
	{
		CCtrlTable::tagAmtData_Touch* pAmtData = (CCtrlTable::tagAmtData_Touch*)pData;
		m_pComponent = pAmtData->pComponent; 

		((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->setParams(new CAmtPoint(0, 0), this);
		
		{
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(0, new CAmtPoint(-5, 0)));
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(25, new CAmtPoint(0, 0)));
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(50, new CAmtPoint(0, -5)));
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(75, new CAmtPoint(0, 0)));
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(100, new CAmtPoint(5, 0)));
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(125, new CAmtPoint(0, 0)));
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(150, new CAmtPoint(0, 5)));
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(175, new CAmtPoint(0, 0)));
		}
		
		return TRUE;
	}

	void CTableAmtTouchShake::onAnimationValueChanged(CBaseAnimation* pSource)
	{
		if( pSource == m_pAnimation && m_pAnimation != NULL)
		{
			CAmtPoint* pAmtPoint = static_cast<CAmtPoint*>(pSource->getAnimationData());

			for (INT j = 0; j < (INT)m_pTable->m_aryShowComponent.size(); j++)
			{
				CComponent* pComponent = m_pTable->m_aryShowComponent[j]->pComponent;
				if (pComponent == m_pComponent)
				{
					m_pTable->m_aryShowComponent[j]->iShowX = m_pTable->m_aryShowComponent[j]->pComponent->getX() + pAmtPoint->m_iX;
					m_pTable->m_aryShowComponent[j]->iShowY = m_pTable->m_aryShowComponent[j]->pComponent->getY() + pAmtPoint->m_iY;
				}
			}

			m_pTable->setDirty();
		}
	}

	void CTableAmtTouchShake::onAnimationEnded(CBaseAnimation* pSource)
	{
		if(m_pTable != NULL)
		{
			m_pTable->stopTouchAnimation();
		}
	}
}