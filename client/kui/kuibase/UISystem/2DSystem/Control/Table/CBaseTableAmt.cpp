#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseTableAmt::CBaseTableAmt()
		: m_pTable(NULL)
		, m_pAnimation(NULL)
	{
		m_animationPlayer.setFPS(200);
	}

	CBaseTableAmt::~CBaseTableAmt()
	{
		;
	}

	void CBaseTableAmt::play()
	{
		if( m_pAnimation != NULL )
		{
			m_animationPlayer.stop();
			m_animationPlayer.play(m_pAnimation, FALSE);
		}
	}

	void CBaseTableAmt::stop()
	{
		m_animationPlayer.stop();
	}

	BOOLEAN CBaseTableAmt::prepareAmtData(Object* pData)
	{
		return FALSE;
	}

	BOOLEAN CBaseTableAmt::prepareAmtDataExt(Object* pData)
	{
		return FALSE;
	}

	void CBaseTableAmt::onAnimationValueChanged(CBaseAnimation* pSource)
	{
		;
	}

	void CBaseTableAmt::onAnimationEnded(CBaseAnimation* pSource)
	{
		;
	}
}