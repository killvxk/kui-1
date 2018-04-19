#include "OpenHMI.h"

namespace OpenHMI
{
	CTableAmtElasticity::CTableAmtElasticity()
		: m_iStartValue(0)
		, m_iEndValue(0)
	{
		m_pAnimation = new CValueAnimation();
	}

	CTableAmtElasticity::~CTableAmtElasticity()
	{
		;
	}

	BOOLEAN CTableAmtElasticity::prepareAmtData(Object* pData)
	{
		CCtrlTable::tagAmtData_Elasiticity* pAmtData = (CCtrlTable::tagAmtData_Elasiticity*)pData;
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

		return TRUE;
	}

	void CTableAmtElasticity::onAnimationValueChanged(CBaseAnimation* pSource)
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
			
			m_pTable->showItems(iPos);

			m_pTable->dispatchEvent(CEventTable::ITEM_SCROLLING);
			
			if (bNeedDeleteTimer)
			{
				m_pTable->stopElasticityAnimation();
			}
		}
	}

	void CTableAmtElasticity::onAnimationEnded(CBaseAnimation* pSource)
	{
		if(m_pTable != NULL)
		{
			m_pTable->stopElasticityAnimation();
		}
	}
}