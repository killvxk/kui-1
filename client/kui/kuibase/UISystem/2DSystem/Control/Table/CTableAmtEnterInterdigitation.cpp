#include "OpenHMI.h"

namespace OpenHMI
{
	CTableAmtEnterInterdigitation::CTableAmtEnterInterdigitation()
	{
		m_pAnimation = new CKeyFrameAnimation();
	}

	CTableAmtEnterInterdigitation::~CTableAmtEnterInterdigitation()
	{
		;
	}

	BOOLEAN CTableAmtEnterInterdigitation::prepareAmtData(Object* pData)
	{
		if( m_pTable->m_aryShowComponent.size() <= 0 )
		{
			return FALSE;
		}
		
		((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->setParams(new CAmtInt(0), this);
			
		for( INT i = 0; i < (INT)m_pTable->m_aryShowComponent.size(); i++ )
		{
			m_pTable->m_aryShowComponent[i]->pComponent->setVisible(FALSE);
			((CKeyFrameAnimation*)((CBaseAnimation*)(m_pAnimation)))->addKeyFrame(new CKeyFrame(0 + 100 * i, new CAmtInt(i)));
		}

		return TRUE;
	}

	void CTableAmtEnterInterdigitation::onAnimationValueChanged(CBaseAnimation* pSource)
	{
		if( pSource == m_pAnimation && m_pAnimation != NULL)
		{
			CAmtInt* pInt = static_cast<CAmtInt*>(pSource->getAnimationData());
			INT iIndex = pInt->m_iValue;

			if( iIndex >= (INT)m_pTable->m_aryShowComponent.size() )
			{
				return;
			}
			
			for( INT i = 0; i < (INT)m_pTable->m_aryShowComponent.size(); i++ )
			{
				if( i == iIndex )
				{
					m_pTable->m_aryShowComponent[i]->pComponent->setVisible(TRUE);
				}
			}
			
			m_pTable->setDirty();
		}
	}

	void CTableAmtEnterInterdigitation::onAnimationEnded(CBaseAnimation* pSource)
	{
		if(m_pTable != NULL)
		{
			m_pTable->stopEnterAnimation();
		}
	}
}