/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CRectangle : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CRectangle();
		CRectangle(const CRectangle &src);
		CRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight);
		virtual ~CRectangle();

		void	set(INT iX, INT iY, UINT uiW, UINT uiH)	{ m_iX = iX; m_iY = iY; m_uiWidth = uiW; m_uiHeight = uiH;	}
		void	setX(INT iValue)		{ m_iX = iValue; }
		INT		getX() const			{ return m_iX; }
		void	setY(INT iValue)		{ m_iY = iValue; }
		INT		getY() const			{ return m_iY; }
		void	setWidth(UINT uiValue)	{ m_uiWidth = uiValue; }
		UINT	getWidth() const		{ return m_uiWidth; }
		void	setHeight(UINT uiValue)	{ m_uiHeight = uiValue; }
		UINT	getHeight() const		{ return m_uiHeight; }
		INT		getRight() const		{ return m_iX + (INT)m_uiWidth - 1; }
		INT		getBottom() const		{ return m_iY + (INT)m_uiHeight - 1; }

		BOOLEAN		isEmpty() const		{ return (m_uiWidth == 0) || (m_uiHeight == 0); }
		void		offset(INT iX, INT iY);		
		BOOLEAN		ptInRect(const CPoint &pt) const;
		BOOLEAN		rectInRect(const CRectangle &rc) const;
		CRectangle	intersect(const CRectangle &rc) const;

		BOOLEAN		isIntersect(const CRectangle &rc) const;
		void		clear();
		void		add(const CRectangle &rc);

		CRectangle& operator =(const CRectangle &src);
		BOOLEAN operator ==(const CRectangle &src) const;
		BOOLEAN operator !=(const CRectangle &src) const;

	protected:// method

	private:// method

	protected:// property

	private:// property
		INT		m_iX;
		INT		m_iY;
		UINT	m_uiWidth;
		UINT	m_uiHeight;
	};
}