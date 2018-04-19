#include "OpenHMI.h"

namespace OpenHMI
{
	CListAmtEnterInterdigitation::CListAmtEnterInterdigitation()
		: m_iActiveIndex(0)
		, m_bInitialized(FALSE)
	{
		m_pAnimation = new CValueAnimation();
	}

	CListAmtEnterInterdigitation::~CListAmtEnterInterdigitation()
	{
		;
	}

	BOOLEAN CListAmtEnterInterdigitation::prepareAmtData(Object* pData)
	{
		if ( m_pList->m_aryShowComponent.size() <= 0 )
		{
			return FALSE;
		}

		((CValueAnimation*)((CBaseAnimation*)(m_pAnimation)))->setParams(new CAmtInt(0), 1000, new CAmtInt(0), new CAmtInt(50), this);
		m_iActiveIndex = 0;
		m_bInitialized = FALSE;

		return TRUE;
	}

	void CListAmtEnterInterdigitation::onAnimationValueChanged(CBaseAnimation* pSource)
	{
		if ( pSource == m_pAnimation && m_pAnimation != NULL)
		{
			CAmtInt* pInt = static_cast<CAmtInt*>(pSource->getAnimationData());
			UINT uiCurrentCount = (UINT)pInt->m_iValue;

			m_iActiveIndex = uiCurrentCount / ( 50 / m_pList->m_aryShowComponent.size() );

			if( m_iActiveIndex >= (INT)m_pList->m_aryShowComponent.size() )
			{
				return;
			}

			if( !m_bInitialized )
			{
				for( INT i = 0; i < (INT)m_pList->m_aryShowComponent.size(); i++ )
				{
					if( i % 2 == 0 )
					{
						m_pList->m_aryShowComponent[i]->iShowX -= m_pList->getPreferredSize().getX();
					}
					else
					{
						m_pList->m_aryShowComponent[i]->iShowX += m_pList->getPreferredSize().getX();
					}
				}

				m_bInitialized = TRUE;
			}

			if( m_iActiveIndex % 2 == 0 )
			{
				m_pList->m_aryShowComponent[m_iActiveIndex]->iShowX =m_pList->m_aryShowComponent[m_iActiveIndex]->iShowX + m_pList->getPreferredSize().getX() / ( 50 / m_pList->m_aryShowComponent.size() );
				if(m_pList->m_aryShowComponent[m_iActiveIndex]->iShowX >= 0 )
				{
					m_pList->m_aryShowComponent[m_iActiveIndex]->iShowX = 0;
				}
			}
			else
			{
				m_pList->m_aryShowComponent[m_iActiveIndex]->iShowX =m_pList->m_aryShowComponent[m_iActiveIndex]->iShowX - m_pList->getPreferredSize().getX() / (50 / m_pList->m_aryShowComponent.size());
				if(m_pList->m_aryShowComponent[m_iActiveIndex]->iShowX <= 0 )
				{
					m_pList->m_aryShowComponent[m_iActiveIndex]->iShowX = 0;
				}
			}

			if( m_iActiveIndex > 0 )
			{
				m_pList->m_aryShowComponent[m_iActiveIndex-1]->iShowX = 0;
			}

			m_pList->setDirty();
		}
	}

	void CListAmtEnterInterdigitation::onAnimationEnded(CBaseAnimation* pSource)
	{
		m_pList->stopEnterAnimation();

		for( INT i = 0; i < (INT)m_pList->m_aryShowComponent.size(); i++ )
		{
			m_pList->m_aryShowComponent[i]->iShowX = m_pList->m_aryShowComponent[i]->pComponent->getX();
		}

		m_pList->setDirty();
	}
}