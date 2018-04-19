#include "OpenHMI.h"

namespace OpenHMI
{
	CRectangleF::CRectangleF() 
		: m_fX(0.0f) 
		, m_fY(0.0f)
		, m_fWidth(0.0f)
		, m_fHeight(0.0f)
	{
		;
	}

	CRectangleF::CRectangleF(const CRectangleF &src)
		: m_fX(src.m_fX)
		, m_fY(src.m_fY)
		, m_fWidth(src.m_fWidth)
		, m_fHeight(src.m_fHeight)
	{
		;
	}

	CRectangleF::CRectangleF(FLOAT fX, FLOAT fY, FLOAT fWidth, FLOAT fHeight)
		: m_fX(fX) 
		, m_fY(fY)
		, m_fWidth(fWidth)
		, m_fHeight(fHeight)
	{
		;
	}

	CRectangleF::~CRectangleF()
	{
		;
	}

	BOOLEAN	CRectangleF::isEmpty() const
	{
		return Math::isZero(m_fWidth) && Math::isZero(m_fHeight);
	}

	void CRectangleF::offset(FLOAT fX, FLOAT fY)
	{
		m_fX += fX;
		m_fY += fY;
	}

	CRectangleF& CRectangleF::operator =(const CRectangleF &src)
	{
		m_fX = src.m_fX;
		m_fY = src.m_fY;
		m_fWidth = src.m_fWidth;
		m_fHeight = src.m_fHeight;

		return *this;
	}

	BOOLEAN CRectangleF::operator ==(const CRectangleF &src) const
	{
		return Math::isZero(m_fX - src.m_fX) &&
			Math::isZero(m_fY - src.m_fY) &&
			Math::isZero(m_fWidth - src.m_fWidth) &&
			Math::isZero(m_fHeight - src.m_fHeight) ;
	}

	BOOLEAN CRectangleF::operator !=(const CRectangleF &src) const
	{
		return m_fX != src.m_fX || m_fY != src.m_fY || m_fWidth != src.m_fWidth || m_fHeight != src.m_fHeight;
	}
}