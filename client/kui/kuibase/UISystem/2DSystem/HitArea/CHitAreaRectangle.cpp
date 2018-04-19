#include "OpenHMI.h"

namespace OpenHMI
{
	CHitAreaRectangle::CHitAreaRectangle() 
		: m_iX(-1)
		, m_iY(-1)
		, m_uiWidth(0)
		, m_uiHeight(0)
	{
		m_eHTAType = HTA_RECTANGLE;
	}

	CHitAreaRectangle::~CHitAreaRectangle()
	{
		;
	}

	BOOLEAN CHitAreaRectangle::hitTest(INT iX, INT iY)
	{
		if (iX >= m_iX - (INT)ENV::getHitOffset() && iX <= m_iX + (INT)m_uiWidth + (INT)ENV::getHitOffset())
		{
			if (iY >= m_iY - (INT)ENV::getHitOffset() && iY <= m_iY + (INT)m_uiHeight + (INT)ENV::getHitOffset())
			{
				return TRUE;
			}
		}

		return FALSE;
	}
}