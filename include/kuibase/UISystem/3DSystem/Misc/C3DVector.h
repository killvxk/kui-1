/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API C3DVector : public Object
	{
	public:// const define
		static	const	C3DVector	ZERO;
		static	const	C3DVector	UNIT_X;
		static	const	C3DVector	UNIT_Y;
		static	const	C3DVector	UNIT_Z;
		static	const	C3DVector	NEGATIVE_UNIT_X;
		static	const	C3DVector	NEGATIVE_UNIT_Y;
		static	const	C3DVector	NEGATIVE_UNIT_Z;
		static	const	C3DVector	UNIT_SCALE;

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		C3DVector();
		C3DVector(const C3DVector &src);
		C3DVector(FLOAT fX, FLOAT fY, FLOAT fZ);
		~C3DVector();

		void			set(FLOAT fx, FLOAT fy, FLOAT fz)	{ m_fX = fx; m_fY = fy; m_fZ = fz;	}
		void			setX(FLOAT fValue)					{ m_fX = fValue; }
		FLOAT			getX() const						{ return m_fX; }
		void			setY(FLOAT fValue)					{ m_fY = fValue; }
		FLOAT			getY() const						{ return m_fY; }
		void			setZ(FLOAT fValue)					{ m_fZ = fValue; }
		FLOAT			getZ() const						{ return m_fZ; }

		FLOAT			length() const;
		FLOAT			squaredLength() const;
		BOOLEAN			isZeroLength() const;
		FLOAT			distance(const C3DVector &vt) const;
		FLOAT			squaredDistance(const C3DVector &vt) const;

		FLOAT			normalise();
		C3DVector		normalisedCopy() const;

		FLOAT			dotProduct(const C3DVector &vt) const;
		FLOAT			absDotProduct(const C3DVector &vt) const;
		C3DVector		crossProduct(const C3DVector &vt) const;

		C3DVector		midPoint(const C3DVector &vt) const;
		void			makeFloor(const C3DVector &vt);
		void			makeCeil(const C3DVector &vt);

		C3DVector		perpendicular() const;
		C3DVector		reflect(const C3DVector &vtNormal) const;
		C3DVector		primaryAxis() const;

		FLOAT			angleBetween(const C3DVector &vt) const;
		C3DQuaternion	getRotationTo(const C3DVector &vtDest, const C3DVector vtFallbackAxis = C3DVector::ZERO) const;
		BOOLEAN			positionEquals(const C3DVector &vt, FLOAT fTolerance = 0.001) const;
		BOOLEAN			positionCloses(const C3DVector &vt, FLOAT fTolerance = 0.001) const;
		BOOLEAN			directionEquals(const C3DVector &vt, FLOAT fTolerance) const;
		
		BOOLEAN			isNaN() const;

		C3DVector&	operator =(const C3DVector &src);
		BOOLEAN		operator ==(const C3DVector &src) const;
		BOOLEAN		operator !=(const C3DVector &src) const;

		C3DVector	operator +(const C3DVector &src) const;
		C3DVector	operator -(const C3DVector &src) const;
		C3DVector	operator *(FLOAT fScalar) const;
		C3DVector	operator *(const C3DVector &src) const;
		C3DVector	operator /(FLOAT fScalar) const;
		C3DVector	operator /(const C3DVector &src) const;

		C3DVector	operator +() const;
		C3DVector	operator -() const;

		C3DVector&	operator +=(const C3DVector &src);
		C3DVector&	operator -=(const C3DVector &src);
		C3DVector&	operator *=(FLOAT fScalar);
		C3DVector&	operator *=(const C3DVector &src);
		C3DVector&	operator /=(FLOAT fScalar);
		C3DVector&	operator /=(const C3DVector &src);

		BOOLEAN operator <(const C3DVector &src) const;
		BOOLEAN operator >(const C3DVector &src) const;

		FLOAT*	ptr()	{ return &m_fX;	}

	protected:// method

	private:// method

	protected:// property

	private:// property
		FLOAT	m_fX;
		FLOAT	m_fY;
		FLOAT	m_fZ;
	};
}