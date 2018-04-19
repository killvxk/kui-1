#include "OpenHMI.h"

namespace OpenHMI
{
	CListAmtTouchShake::CListAmtTouchShake()
		: m_pComponent(NULL)
	{
		m_pAnimation = new CKeyFrameAnimation();
	}

	CListAmtTouchShake::~CListAmtTouchShake()
	{
		;
	}

	BOOLEAN CListAmtTouchShake::prepareAmtData(Object* pData)
	{
		CCtrlList::tagAmtData_Touch* pAmtData = (CCtrlList::tagAmtData_Touch*)pData;
		m_pComponent = pAmtData->pComponent;

		((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->setParams(new CAmtInt(0), this);
		((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(0, new CAmtInt(0)));
		for( INT i=0; i< 1; i++)
		{
			INT iItemSpace = m_pList->m_iItemSpace > 0 ? m_pList->m_iItemSpace : 10;

			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(25 + 100 * i, new CAmtInt(iItemSpace / 1 + i)));
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(50 + 100 * i, new CAmtInt(0)));
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(75 + 100 * i, new CAmtInt(-iItemSpace / 1 + i)));
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(100 + 100 * i, new CAmtInt(0)));
		}

		return TRUE;
	}

	void CListAmtTouchShake::onAnimationValueChanged(CBaseAnimation* pSource)
	{
		if ( pSource == m_pAnimation && m_pAnimation != NULL)
		{
			CAmtInt* pInt = static_cast<CAmtInt*>(pSource->getAnimationData());

			for (INT j = 0; j < (INT)m_pList->m_aryShowComponent.size(); j++)
			{
				CComponent* pComponent = m_pList->m_aryShowComponent[j]->pComponent;
				if (pComponent == m_pComponent)
				{
					m_pList->m_aryShowComponent[j]->iShowY = m_pList->m_aryShowComponent[j]->pComponent->getY() + pInt->m_iValue;
					break;
				}
			}
			m_pList->setDirty();
		}
	}

	void CListAmtTouchShake::onAnimationEnded(CBaseAnimation* pSource)
	{
		if(m_pList != NULL)
		{
			m_pList->stopTouchAnimation();
		}
	}
}