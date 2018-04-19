/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CPoint : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CPoint();
		CPoint(const CPoint &src);
		CPoint(INT iX, INT iY);
		virtual ~CPoint();

		void	setX(INT iValue)	{ m_iX = iValue; }
		INT		getX() const		{ return m_iX; }
		void	setY(INT iValue)	{ m_iY = iValue; }
		INT		getY() const		{ return m_iY; }
		void	offset(INT iX, int iY);

		CPoint& operator =(const CPoint &src);
		BOOLEAN operator ==(const CPoint &src) const;
		BOOLEAN operator !=(const CPoint &src) const;

	protected:// method

	private:// method

	protected:// property

	private:// property
		INT		m_iX;
		INT		m_iY;
	};
}