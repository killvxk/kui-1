/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CSize : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CSize();
		CSize(const CSize &src);
		CSize(INT iX, INT iY);
		virtual ~CSize();

		void	setX(INT iValue)	{ m_iX = iValue; }
		INT		getX() const		{ return m_iX; }
		void	setY(INT iValue)	{ m_iY = iValue; }
		INT		getY() const		{ return m_iY; }

		CSize& operator =(const CSize &src);
		BOOLEAN operator ==(const CSize &src) const;
		BOOLEAN operator !=(const CSize &src) const;

	protected:// method

	private:// method

	protected:// property

	private:// property
		INT		m_iX;
		INT		m_iY;
	};
}