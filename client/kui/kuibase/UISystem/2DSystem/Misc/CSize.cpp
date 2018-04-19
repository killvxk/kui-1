#include "OpenHMI.h"

namespace OpenHMI
{
	CSize::CSize() 
		: m_iX(0)
		, m_iY(0)
	{
		;
	}

	CSize::CSize(const CSize &src) 
		: m_iX(src.m_iX)
		, m_iY(src.m_iY)
	{
		;
	}

	CSize::CSize(INT iX, INT iY) 
		: m_iX(iX)
		, m_iY(iY)
	{
		;
	}

	CSize::~CSize()
	{
		;
	}

	CSize& CSize::operator =(const CSize &src)
	{
		m_iX = src.m_iX;
		m_iY = src.m_iY;

		return *this;
	}

	BOOLEAN CSize::operator ==(const CSize &src) const
	{
		return m_iX == src.m_iX && m_iY == src.m_iY;
	}

	BOOLEAN CSize::operator !=(const CSize &src) const
	{
		return m_iX != src.m_iX || m_iY != src.m_iY;
	}
}