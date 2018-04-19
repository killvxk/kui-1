#include "OpenHMI.h"

namespace OpenHMI
{
	CTableAmtFling::CTableAmtFling()
		: m_iStartValue(0)
		, m_fFlingSpeed(0.0f)
		, m_iStartPos(0)
		, m_iDeltaPos(0)
	{
		m_pAnimation = new CValueAnimation();
	}

	CTableAmtFling::~CTableAmtFling()
	{
		;
	}

	BOOLEAN CTableAmtFling::prepareAmtData(Object* pData)
	{
		CCtrlTable::tagAmtData_Fling* pAmtData = (CCtrlTable::tagAmtData_Fling*)pData;
		m_iStartValue = pAmtData->iStartValue;
		INT	iSpeedY = pAmtData->iSpeed;

		if (-FLING_START_SPEED < iSpeedY && iSpeedY < FLING_START_SPEED)
		{
			return FALSE;
		}
		
		if (iSpeedY < 0)
		{
			iSpeedY = -iSpeedY;
		}
		
		m_iStartValue = m_iStartValue;
		
		m_fFlingSpeed = (FLOAT)iSpeedY / (FLING_DURATION_TIME * FLING_DURATION_TIME);

		((CValueAnimation*)((CBaseAnimation*)(m_pAnimation)))->setParams(new CAmtInt(0), 25*FLING_DURATION_TIME, new CAmtInt(0), new CAmtInt(FLING_DURATION_TIME), this);

		return TRUE;
	}

	BOOLEAN CTableAmtFling::prepareAmtDataExt(Object* pData)
	{
		CCtrlTable::tagAmtData_FlingExt* pAmtData = (CCtrlTable::tagAmtData_FlingExt*)pData;

		m_iStartPos = pAmtData->iStartPos;
		m_iDeltaPos = pAmtData->iDeltaPos;

		//((CValueAnimation*)((CBaseAnimation*)(m_pAnimation)))->setInterpolation(new CBezierInterpolation(0.000f, 0.000f, 0.000f, 1.000f));
		((CValueAnimation*)((CBaseAnimation*)(m_pAnimation)))->setParams(new CAmtInt(0), 200, new CAmtInt(0), new CAmtInt(pAmtData->iDeltaPos), this);

		return TRUE;
	}

	void CTableAmtFling::onAnimationValueChanged(CBaseAnimation* pSource)
	{
		if( pSource == m_pAnimation && m_pAnimation != NULL)
		{
#if 0
			CAmtInt* pInt = static_cast<CAmtInt*>(pSource->getAnimationData());
			UINT uiCurrentCount = (UINT)pInt->m_iValue;

			INT iDeltaY =   (INT)((-m_fFlingSpeed) 
							  * (INT)(uiCurrentCount - FLING_DURATION_TIME) 
							  * (INT)(uiCurrentCount - FLING_DURATION_TIME)
							  + m_fFlingSpeed * FLING_DURATION_TIME * FLING_DURATION_TIME);
			
			BOOLEAN bNeedDeleteTimer = FALSE;
			
			INT iPos = 0;
			if (m_pTable->m_bDirectionDown)
			{
				iPos = m_iStartValue + iDeltaY;
				if (iPos >= (INT)m_pTable->getTotalShowItemHeight() - m_pTable->getPreferredSize().getY())
				{
					iPos = m_pTable->getTotalShowItemHeight() - m_pTable->getPreferredSize().getY();
					bNeedDeleteTimer = TRUE;
				}
			}
			else
			{
				iPos = m_iStartValue - iDeltaY;
				if (iPos <= 0)
				{
					iPos = 0;
					bNeedDeleteTimer = TRUE;
				}
			}
			
			if (m_pTable->m_pScrollbar != NULL)
			{
				m_pTable->m_pScrollbar->getModel()->setPosition(iPos);
				m_pTable->m_pScrollbar->refresh();
			}
			
			if (m_pTable->needVerifyShowInfo(iPos))
			{
				m_pTable->verifyShowInfo();
				m_pTable->showItemState();
			}
			
			m_pTable->showItems(iPos);
			
			m_pTable->dispatchEvent(CEventTable::ITEM_SCROLLING);
			
			if (bNeedDeleteTimer || uiCurrentCount == FLING_DURATION_TIME)
			{
				m_pTable->stopFlingAnimation();
			}
#endif
			CAmtInt* pInt = static_cast<CAmtInt*>(pSource->getAnimationData());

			INT iPos = 0;
			iPos = m_iStartPos - pInt->m_iValue;

			if (m_pTable->m_pScrollbar != NULL)
			{
				m_pTable->m_pScrollbar->getModel()->setPosition(iPos);
				m_pTable->m_pScrollbar->refresh();
			}

			if (m_pTable->needVerifyShowInfo(iPos))
			{
				m_pTable->verifyShowInfo();
				m_pTable->showItemState();
			}

			m_pTable->showItems(iPos);

			m_pTable->dispatchEvent(CEventTable::ITEM_SCROLLING);

		}
	}

	void CTableAmtFling::onAnimationEnded(CBaseAnimation* pSource)
	{
		m_pTable->showItems(m_iStartPos - m_iDeltaPos);
		if(m_pTable != NULL)
		{
			m_pTable->stopFlingAnimation();
		}
	}
}