#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseListAmt::CBaseListAmt()
		: m_pList(NULL)
		, m_pListSimple(NULL)
		, m_pAnimation(NULL)
	{
		m_animationPlayer.setFPS(100);
	}

	CBaseListAmt::~CBaseListAmt()
	{
		;
	}

	void CBaseListAmt::play()
	{
		if( m_pAnimation != NULL )
		{
			m_animationPlayer.stop();
			m_animationPlayer.play(m_pAnimation, FALSE);
		}
	}

	void CBaseListAmt::stop()
	{
		m_animationPlayer.stop();
	}

	BOOLEAN CBaseListAmt::prepareAmtData(Object* pData)
	{
		return FALSE;
	}

	BOOLEAN CBaseListAmt::prepareAmtDataExt(Object* pData)
	{
		return FALSE;
	}

	void CBaseListAmt::onAnimationValueChanged(CBaseAnimation* pSource)
	{
		;
	}

	void CBaseListAmt::onAnimationEnded(CBaseAnimation* pSource)
	{
		;
	}
}