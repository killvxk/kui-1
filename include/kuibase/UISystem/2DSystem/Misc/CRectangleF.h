/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CRectangleF : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CRectangleF();
		CRectangleF(const CRectangleF &src);
		CRectangleF(FLOAT fX, FLOAT fY, FLOAT fWidth, FLOAT fHeight);
		virtual ~CRectangleF();

		void	set(FLOAT fX, FLOAT fY, FLOAT fW, FLOAT fH)	{ m_fX = fX; m_fY = fY; m_fWidth = fW; m_fHeight = fH;	}
		void	setX(FLOAT fValue)		{ m_fX = fValue; }
		FLOAT	getX() const			{ return m_fX; }
		void	setY(FLOAT fValue)		{ m_fY = fValue; }
		FLOAT	getY() const			{ return m_fY; }
		void	setWidth(FLOAT fValue)	{ m_fWidth = fValue; }
		FLOAT	getWidth() const		{ return m_fWidth; }
		void	setHeight(FLOAT fValue)	{ m_fHeight = fValue; }
		FLOAT	getHeight() const		{ return m_fHeight; }
		FLOAT	getRight() const		{ return m_fX + m_fWidth; }
		FLOAT	getBottom() const		{ return m_fY + m_fHeight; }

		BOOLEAN		isEmpty() const;
		void		offset(FLOAT fX, FLOAT fY);		


		CRectangleF& operator =(const CRectangleF &src);
		BOOLEAN operator ==(const CRectangleF &src) const;
		BOOLEAN operator !=(const CRectangleF &src) const;

	protected:// method

	private:// method

	protected:// property

	private:// property
		FLOAT	m_fX;
		FLOAT	m_fY;
		FLOAT	m_fWidth;
		FLOAT	m_fHeight;
	};
}