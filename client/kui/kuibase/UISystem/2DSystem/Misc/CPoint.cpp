#include "OpenHMI.h"

namespace OpenHMI
{
	CPoint::CPoint()
		: m_iX(0)
		, m_iY(0)
	{
		;
	}

	CPoint::CPoint(const CPoint &src) 
		: m_iX(src.m_iX)
		, m_iY(src.m_iY)
	{
		;
	}

	CPoint::CPoint(INT iX, INT iY)
		: m_iX(iX)
		, m_iY(iY)
	{
		;
	}

	CPoint::~CPoint()
	{
		;
	}

	void CPoint::offset(INT iX, int iY)
	{
		m_iX += iX;
		m_iY += iY;
	}

	CPoint& CPoint::operator =(const CPoint &src)
	{
		m_iX = src.m_iX;
		m_iY = src.m_iY;

		return *this;
	}

	BOOLEAN CPoint::operator ==(const CPoint &src) const
	{
		return m_iX == src.m_iX && m_iY == src.m_iY;
	}

	BOOLEAN CPoint::operator !=(const CPoint &src) const
	{
		return m_iX != src.m_iX || m_iY != src.m_iY;
	}
}