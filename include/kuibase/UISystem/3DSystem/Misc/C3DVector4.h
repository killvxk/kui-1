/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API C3DVector4 : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		C3DVector4();
		C3DVector4(const C3DVector4 &src);
		C3DVector4(FLOAT fX, FLOAT fY, FLOAT fZ,FLOAT fW);
		virtual ~C3DVector4();

		void			setX(FLOAT fValue)	{ m_fX = fValue; }
		FLOAT			getX() const		{ return m_fX; }
		void			setY(FLOAT fValue)	{ m_fY = fValue; }
		FLOAT			getY() const		{ return m_fY; }
		void			setZ(FLOAT fValue)	{ m_fZ = fValue; }
		FLOAT			getZ() const		{ return m_fZ; }
		void			setW(FLOAT fValue)	{ m_fW = fValue; }
		FLOAT			getW() const		{ return m_fW;	}

		FLOAT			length() const;
		FLOAT			squaredLength() const;
		BOOLEAN			isZeroLength() const;
		FLOAT			distance(const C3DVector4 &vt) const;
		FLOAT			squaredDistance(const C3DVector4 &vt) const;

		FLOAT			normalise();
		C3DVector4		normalisedCopy() const;

		FLOAT			dotProduct(const C3DVector4 &vt) const;
		FLOAT			absDotProduct(const C3DVector4 &vt) const;

		BOOLEAN			isNaN() const;

		C3DVector4&	operator =(const C3DVector4 &src);
		BOOLEAN		operator ==(const C3DVector4 &src) const;
		BOOLEAN		operator !=(const C3DVector4 &src) const;

		C3DVector4	operator +(const C3DVector4 &src) const;
		C3DVector4	operator -(const C3DVector4 &src) const;
		C3DVector4	operator *(FLOAT fScalar) const;
		C3DVector4	operator *(const C3DVector4 &src) const;
		C3DVector4	operator /(FLOAT fScalar) const;
		C3DVector4	operator /(const C3DVector4 &src) const;

		C3DVector4	operator +() const;
		C3DVector4	operator -() const;

		C3DVector4&	operator +=(const C3DVector4 &src);
		C3DVector4&	operator -=(const C3DVector4 &src);
		C3DVector4&	operator *=(FLOAT fScalar);
		C3DVector4&	operator *=(const C3DVector4 &src);
		C3DVector4&	operator /=(FLOAT fScalar);
		C3DVector4&	operator /=(const C3DVector4 &src);

		BOOLEAN operator <(const C3DVector4 &src) const;
		BOOLEAN operator >(const C3DVector4 &src) const;

	protected:// method

	private:// method

	protected:// property

	private:// property
		FLOAT	m_fX;
		FLOAT	m_fY;
		FLOAT	m_fZ;
		FLOAT	m_fW;
	};
}