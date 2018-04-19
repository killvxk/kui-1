#include "OpenHMI.h"

namespace OpenHMI
{
	CListAmtFling::CListAmtFling()
		: m_iStartValue(0)
		, m_fFlingSpeed(0.0f)
	{
		m_pAnimation = new CValueAnimation();
	}

	CListAmtFling::~CListAmtFling()
	{
		;
	}

	BOOLEAN CListAmtFling::prepareAmtData(Object* pData)
	{
		if (m_pList != NULL)
		{
			CCtrlList::tagAmtData_Fling* pAmtData = (CCtrlList::tagAmtData_Fling*)pData;
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

			//m_timerFling.setDelay( 25 );
			//m_timerFling.setRepeatCount(FLING_DURATION_TIME);
			//m_timerFling.start();

			((CValueAnimation*)((CBaseAnimation*)(m_pAnimation)))->setParams(new CAmtInt(0), 25*FLING_DURATION_TIME, new CAmtInt(0), new CAmtInt(FLING_DURATION_TIME), this);
		}
		else if  (m_pListSimple != NULL)
		{
			CCtrlList_Simple::tagAmtData_Fling* pAmtData = (CCtrlList_Simple::tagAmtData_Fling*)pData;
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

			//m_timerFling.setDelay( 25 );
			//m_timerFling.setRepeatCount(FLING_DURATION_TIME);
			//m_timerFling.start();

			((CValueAnimation*)((CBaseAnimation*)(m_pAnimation)))->setParams(new CAmtInt(0), 25*FLING_DURATION_TIME, new CAmtInt(0), new CAmtInt(FLING_DURATION_TIME), this);
		}

		return TRUE;
	}

	BOOLEAN CListAmtFling::prepareAmtDataExt(Object* pData)
	{
		CCtrlList_Simple::tagAmtData_FlingExt* pAmtData = (CCtrlList_Simple::tagAmtData_FlingExt*)pData;

		m_iStartPos = pAmtData->iStartPos;
		m_iDeltaPos = pAmtData->iDeltaPos;

		//((CValueAnimation*)((CBaseAnimation*)(m_pAnimation)))->setInterpolation(new CBezierInterpolation(0.000f, 0.000f, 0.000f, 1.000f));
		((CValueAnimation*)((CBaseAnimation*)(m_pAnimation)))->setParams(new CAmtInt(0), 200, new CAmtInt(0), new CAmtInt(pAmtData->iDeltaPos), this);

		return TRUE;
	}

	void CListAmtFling::onAnimationValueChanged(CBaseAnimation* pSource)
	{
		if (m_pList != NULL)
		{
			if ( pSource == m_pAnimation && m_pAnimation != NULL)
			{
				CAmtInt* pInt = static_cast<CAmtInt*>(pSource->getAnimationData());
				UINT uiCurrentCount = (UINT)pInt->m_iValue;

				INT iDeltaY =   (INT)((-m_fFlingSpeed) 
					* (INT)(uiCurrentCount - FLING_DURATION_TIME) 
					* (INT)(uiCurrentCount - FLING_DURATION_TIME)
					+ m_fFlingSpeed * FLING_DURATION_TIME * FLING_DURATION_TIME);

				BOOLEAN bNeedDeleteTimer = FALSE;

				INT iPos = 0;
				if (m_pList->m_bDirectionDown)
				{
					iPos = m_iStartValue + iDeltaY;
					if (iPos >= (INT)m_pList->getTotalShowItemHeight() - m_pList->getPreferredSize().getY())
					{
						iPos = m_pList->getTotalShowItemHeight() - m_pList->getPreferredSize().getY();
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

				if (m_pList->m_pScrollbar != NULL)
				{
					m_pList->m_pScrollbar->getModel()->setPosition(iPos);
					m_pList->m_pScrollbar->refresh();
				}

				if (m_pList->needVerifyShowInfo(iPos))
				{
					m_pList->verifyShowInfo();				
					m_pList->showItemState();
				}

				m_pList->showItems(iPos);
				m_pList->m_bShowBubble = TRUE;
				m_pList->showBubble();

				m_pList->dispatchEvent(CEventList::ITEM_SCROLLING);
				m_pList->checkNeedMorePage();

				if (bNeedDeleteTimer || uiCurrentCount == FLING_DURATION_TIME)
				{
					m_pList->stopFlingAnimation();
				}
			}
		}
		else if (m_pListSimple != NULL)
		{
			if ( pSource == m_pAnimation && m_pAnimation != NULL)
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
				if (m_pListSimple->m_bDirectionDown)
				{
					iPos = m_iStartValue + iDeltaY;
					if (iPos >= (INT)m_pListSimple->getTotalShowItemHeight() - m_pListSimple->getPreferredSize().getY())
					{
						iPos = m_pListSimple->getTotalShowItemHeight() - m_pListSimple->getPreferredSize().getY();
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

				if (m_pListSimple->m_pScrollbar != NULL)
				{
					m_pListSimple->m_pScrollbar->getModel()->setPosition(iPos);
					m_pListSimple->m_pScrollbar->refresh();
				}

				if (m_pListSimple->needVerifyShowInfo(iPos))
				{
					m_pListSimple->verifyShowInfo();				
					m_pListSimple->showItemState();
				}

				m_pListSimple->showItems(iPos);

				m_pListSimple->dispatchEvent(CEventList::ITEM_SCROLLING);
				m_pListSimple->checkNeedMorePage();

				if (bNeedDeleteTimer || uiCurrentCount == FLING_DURATION_TIME)
				{
					m_pListSimple->stopFlingAnimation();
				}
#endif
				CAmtInt* pInt = static_cast<CAmtInt*>(pSource->getAnimationData());

				INT iPos = 0;
				iPos = m_iStartPos - pInt->m_iValue;

				if (m_pListSimple->m_pScrollbar != NULL)
				{
					m_pListSimple->m_pScrollbar->getModel()->setPosition(iPos);
					m_pListSimple->m_pScrollbar->refresh();
				}

				if (m_pListSimple->needVerifyShowInfo(iPos))
				{
					m_pListSimple->verifyShowInfo();
					m_pListSimple->showItemState();
				}

				m_pListSimple->showItems(iPos);

				m_pListSimple->dispatchEvent(CEventList::ITEM_SCROLLING);
			}
		}		
	}

	void CListAmtFling::onAnimationEnded(CBaseAnimation* pSource)
	{
		if (m_pList != NULL)
		{
			m_pList->stopFlingAnimation();
		}
		else if (m_pListSimple != NULL)
		{
			m_pListSimple->stopFlingAnimation();
		}
	}
}