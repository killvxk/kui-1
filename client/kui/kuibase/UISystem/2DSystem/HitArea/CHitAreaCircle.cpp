#include "OpenHMI.h"

namespace OpenHMI
{
	CHitAreaCircle::CHitAreaCircle()
		: m_iX(-1)
		, m_iY(-1)
		, m_uiRadius(0)
	{
		m_eHTAType = HTA_CIRCLE;
	}

	CHitAreaCircle::~CHitAreaCircle()
	{
		;
	}

	void CHitAreaCircle::setCenterPoint(INT iX, INT iY)
	{
		m_iX = iX;
		m_iY = iY;
	}

	BOOLEAN CHitAreaCircle::hitTest(INT iX, INT iY)
	{
		if( m_uiRadius <= 0)
		{
			return FALSE;
		}

		if ( (UINT)((m_iX - iX) * (m_iX - iX) + (m_iY - iY) * (m_iY - iY)) <= (m_uiRadius + ENV::getHitOffset()) * (m_uiRadius + ENV::getHitOffset()) )
		{
			return TRUE;
		}

		return FALSE;
	}
}