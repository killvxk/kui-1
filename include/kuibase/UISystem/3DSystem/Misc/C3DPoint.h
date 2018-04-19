/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API C3DPoint3 : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		C3DPoint3();
		C3DPoint3(const C3DPoint3 &src);
		C3DPoint3(FLOAT fX, FLOAT fY, FLOAT fZ);
		virtual ~C3DPoint3();

		void	setX(FLOAT fValue)	{ m_fX = fValue; }
		FLOAT	getX() const		{ return m_fX; }
		void	setY(FLOAT fValue)	{ m_fY = fValue; }
		FLOAT	getY() const		{ return m_fY; }
		void	setZ(FLOAT fValue)	{ m_fZ = fValue; }
		FLOAT	getZ() const		{ return m_fZ; }

		void	offset(FLOAT fX, FLOAT fY, FLOAT fZ);

		C3DPoint3&	operator =(const C3DPoint3 &src);
		BOOLEAN		operator ==(const C3DPoint3 &src) const;
		BOOLEAN		operator !=(const C3DPoint3 &src) const;
		
	protected:// method

	private:// method

	protected:// property

	private:// property
		FLOAT	m_fX;
		FLOAT	m_fY;
		FLOAT	m_fZ;
	};
}