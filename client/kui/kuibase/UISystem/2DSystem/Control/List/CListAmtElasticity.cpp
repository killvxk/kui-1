#include "OpenHMI.h"

namespace OpenHMI
{
	CListAmtElasticity::CListAmtElasticity()
		: m_iStartValue(0)
		, m_iEndValue(0)
	{
		m_pAnimation = new CValueAnimation();
	}

	CListAmtElasticity::~CListAmtElasticity()
	{
		;
	}

	BOOLEAN CListAmtElasticity::prepareAmtData(Object* pData)
	{
		if (m_pList != NULL)
		{ 
			CCtrlList::tagAmtData_Elasiticity* pAmtData = (CCtrlList::tagAmtData_Elasiticity*)pData;
			m_iStartValue = pAmtData->iStartValue;
			m_iEndValue = pAmtData->iEndValue;

			m_iStep = 6;
			INT iCount = 0;
			if (m_iEndValue > m_iStartValue)
			{
				iCount = (m_iEndValue - m_iStartValue) / m_iStep + 1;
			}
			else
			{
				iCount = (m_iStartValue -m_iEndValue) / m_iStep + 1;
				m_iStep = -m_iStep;
			}

			((CValueAnimation*)((CBaseAnimation*)(m_pAnimation)))->setParams(new CAmtInt(0), (iCount+1)*10, new CAmtInt(m_iStartValue), new CAmtInt(m_iEndValue), this);
		}
		else
		{
			CCtrlList_Simple::tagAmtData_Elasiticity* pAmtData = (CCtrlList_Simple::tagAmtData_Elasiticity*)pData;
			m_iStartValue = pAmtData->iStartValue;
			m_iEndValue = pAmtData->iEndValue;

			m_iStep = 6;
			INT iCount = 0;
			if (m_iEndValue > m_iStartValue)
			{
				iCount = (m_iEndValue - m_iStartValue) / m_iStep + 1;
			}
			else
			{
				iCount = (m_iStartValue -m_iEndValue) / m_iStep + 1;
				m_iStep = -m_iStep;
			}

			((CValueAnimation*)((CBaseAnimation*)(m_pAnimation)))->setParams(new CAmtInt(0), (iCount+1)*10, new CAmtInt(m_iStartValue), new CAmtInt(m_iEndValue), this);
		}

		return TRUE;
	}

	void CListAmtElasticity::onAnimationValueChanged(CBaseAnimation* pSource)
	{
		if (m_pList != NULL)
		{
			if( pSource == m_pAnimation && m_pAnimation != NULL)
			{
				CAmtInt* pInt = static_cast<CAmtInt*>(pSource->getAnimationData());
				BOOLEAN bNeedDeleteTimer = FALSE;

				INT iPos = pInt->m_iValue;
				if (m_iStep > 0)
				{
					if (iPos >= m_iEndValue)
					{
						iPos = m_iEndValue;
						bNeedDeleteTimer = TRUE;
					}
				}
				else
				{
					if (iPos <= m_iEndValue)
					{
						iPos = m_iEndValue;
						bNeedDeleteTimer = TRUE;
					}
				}
				
				m_pList->showItems(iPos);
				m_pList->m_bShowBubble = TRUE;
				m_pList->showBubble();

				m_pList->dispatchEvent(CEventList::ITEM_SCROLLING);
				
				if (bNeedDeleteTimer)
				{
					m_pList->stopElasticityAnimation();
				}
			}
		}
		else if (m_pListSimple != NULL)
		{
			if( pSource == m_pAnimation && m_pAnimation != NULL)
			{
				CAmtInt* pInt = static_cast<CAmtInt*>(pSource->getAnimationData());
				BOOLEAN bNeedDeleteTimer = FALSE;

				INT iPos = pInt->m_iValue;
				if (m_iStep > 0)
				{
					if (iPos >= m_iEndValue)
					{
						iPos = m_iEndValue;
						bNeedDeleteTimer = TRUE;
					}
				}
				else
				{
					if (iPos <= m_iEndValue)
					{
						iPos = m_iEndValue;
						bNeedDeleteTimer = TRUE;
					}
				}

				m_pListSimple->showItems(iPos);

				m_pListSimple->dispatchEvent(CEventList::ITEM_SCROLLING);

				if (bNeedDeleteTimer)
				{
					m_pListSimple->stopElasticityAnimation();
				}
			}
		}
	}

	void CListAmtElasticity::onAnimationEnded(CBaseAnimation* pSource)
	{
		if (m_pList != NULL)
		{
			m_pList->stopElasticityAnimation();
		}
		else if (m_pListSimple != NULL)
		{
			m_pListSimple->stopElasticityAnimation();
		}
	}
}