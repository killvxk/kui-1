#include "OpenHMI.h"

namespace OpenHMI
{
	CRectangle::CRectangle() 
		: m_iX(0) 
		, m_iY(0)
		, m_uiWidth(0)
		, m_uiHeight(0)
	{
		;
	}

	CRectangle::CRectangle(const CRectangle &src)
		: m_iX(src.m_iX)
		, m_iY(src.m_iY)
		, m_uiWidth(src.m_uiWidth)
		, m_uiHeight(src.m_uiHeight)
	{
		;
	}

	CRectangle::CRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight)
		: m_iX(iX) 
		, m_iY(iY)
		, m_uiWidth(uiWidth)
		, m_uiHeight(uiHeight)
	{
		;
	}

	CRectangle::~CRectangle()
	{
		;
	}

	void CRectangle::offset(INT iX, INT iY)
	{
		m_iX += iX;
		m_iY += iY;
	}

	BOOLEAN	CRectangle::ptInRect(const CPoint &pt) const
	{
		if (pt.getX() >= m_iX && pt.getX() <= m_iX + (INT)m_uiWidth - 1)
		{
			if (pt.getY() >= m_iY && pt.getY() <= m_iY + (INT)m_uiHeight - 1)
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	BOOLEAN	CRectangle::rectInRect(const CRectangle &rc) const
	{
		if (m_iX <= rc.m_iX && rc.m_iX + (INT)rc.m_uiWidth <= m_iX + (INT)m_uiWidth)
		{
			if (m_iY <= rc.m_iY && rc.m_iY + (INT)rc.m_uiHeight <= m_iY + (INT)m_uiHeight)
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	CRectangle CRectangle::intersect(const CRectangle &rc) const
	{
		CRectangle rcTemp(0, 0, 0, 0);
		
		// check for total exclusion
		if ((getRight() >= rc.m_iX) && (m_iX <= rc.getRight()) &&
			(getBottom() >= rc.m_iY) && (m_iY <= rc.getBottom()))
		{
			INT iTemp;

			// fill in rcTemp with the intersection
			rcTemp.m_iX = (m_iX > rc.m_iX) ? m_iX : rc.m_iX;
			
			iTemp = (getRight() < rc.getRight()) ? getRight() : rc.getRight();
			rcTemp.m_uiWidth = iTemp - rcTemp.m_iX + 1;

			rcTemp.m_iY = (m_iY > rc.m_iY) ? m_iY : rc.m_iY;
			
			iTemp = (getBottom() < rc.getBottom()) ? getBottom() : rc.getBottom();
			rcTemp.m_uiHeight = iTemp - rcTemp.m_iY + 1;
		}

		return rcTemp;
	}

	BOOLEAN	CRectangle::isIntersect(const CRectangle &rc) const
	{
		INT iRight = getRight();
		INT iBottom = getBottom();
		INT iRCRight = rc.getRight();
		INT iRCBottom = rc.getBottom();

		if ( ( iRight < rc.m_iX) || ( iRCRight < m_iX ) ||
			 ( iBottom < rc.m_iY) || ( iRCBottom < m_iY ) )
		{
			return FALSE;
		}

		return TRUE;
	}

	void CRectangle::clear()
	{
		m_iX = 0;
		m_iY = 0;
		m_uiWidth = 0;
		m_uiHeight = 0;
	}

	void CRectangle::add(const CRectangle &rc)
	{
		if( isEmpty() )
		{
			m_iX = rc.m_iX;
			m_iY = rc.m_iY;
			m_uiWidth = rc.m_uiWidth;
			m_uiHeight = rc.m_uiHeight;
		}
		else
		{
			INT iRight = getRight();
			INT iBottom = getBottom();
			INT iRCRight = rc.getRight();
			INT iRCBottom = rc.getBottom();

			if( m_iX > rc.m_iX )
			{
				m_iX = rc.m_iX;
			}

			if( m_iY > rc.m_iY )
			{
				m_iY = rc.m_iY;
			}

			m_uiWidth = (iRight < iRCRight) ? (iRCRight - m_iX) : (iRight - m_iX);
			m_uiWidth += 1;
			
			m_uiHeight = (iBottom < iRCBottom) ? (iRCBottom - m_iY) : (iBottom - m_iY);
			m_uiHeight += 1;
		}
	}

	CRectangle& CRectangle::operator =(const CRectangle &src)
	{
		m_iX = src.m_iX;
		m_iY = src.m_iY;
		m_uiWidth = src.m_uiWidth;
		m_uiHeight = src.m_uiHeight;

		return *this;
	}

	BOOLEAN CRectangle::operator ==(const CRectangle &src) const
	{
		return m_iX == src.m_iX && m_iY == src.m_iY && m_uiWidth == src.m_uiWidth && m_uiHeight == src.m_uiHeight;
	}

	BOOLEAN CRectangle::operator !=(const CRectangle &src) const
	{
		return m_iX != src.m_iX || m_iY != src.m_iY || m_uiWidth != src.m_uiWidth || m_uiHeight != src.m_uiHeight;
	}
}