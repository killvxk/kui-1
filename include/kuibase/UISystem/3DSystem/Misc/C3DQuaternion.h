/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API C3DQuaternion : public Object
	{
	public:// const define
		static	const	C3DQuaternion	ZERO;
		static	const	C3DQuaternion	IDENTITY;

	private:// const define
		static	const	FLOAT	EPSILON_VALUE;

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		C3DQuaternion();
		C3DQuaternion(const C3DQuaternion &src);
		C3DQuaternion(FLOAT fX, FLOAT fY, FLOAT fZ, FLOAT fW);
		C3DQuaternion(const C3DVector &vtAxis, FLOAT fAngle);
		C3DQuaternion(const C3DVector &vtAxisX, const C3DVector &vtAxisY, const C3DVector &vtAxisZ);
		virtual ~C3DQuaternion();

		static	C3DQuaternion	slerp(FLOAT fT, const C3DQuaternion &qtnP, const C3DQuaternion &qtnQ, BOOLEAN bShortestPath = FALSE);
		static	C3DQuaternion	slerpExtraSpins(FLOAT fT, const C3DQuaternion &qtnP, const C3DQuaternion &qtnQ, INT iExtraSpins);
		static	void			intermediate(const C3DQuaternion &qtn0, const C3DQuaternion &qtn1, const C3DQuaternion &qtn2, C3DQuaternion &qtnA, C3DQuaternion &qtnB);
		static	C3DQuaternion	squad(FLOAT fT, const C3DQuaternion &qtnP, const C3DQuaternion &qtnA, const C3DQuaternion &qtnB, const C3DQuaternion &qtnQ, BOOLEAN bShortestPath = FALSE);
		static	C3DQuaternion	nlerp(FLOAT fT, const C3DQuaternion &qtnP, const C3DQuaternion &qtnQ, BOOLEAN bShortestPath = FALSE);

		void			fromAxisAngle(const C3DVector &vtAxis, FLOAT fAngle);
		void			toAxisAngle(C3DVector &vtAxis, FLOAT &fAngle) const;
		void			fromAxes(const C3DVector &vtAxisX, const C3DVector &vtAxisY, const C3DVector &vtAxisZ);
		void			toAxes(C3DVector &vtAxisX, C3DVector &vtAxisY, C3DVector &vtAxisZ) const;

		C3DVector		xAxis() const;
		C3DVector		yAxis() const;
		C3DVector		zAxis() const;

		FLOAT			getRoll(BOOLEAN bReprojectAxis = TRUE) const;
		FLOAT			getPitch(BOOLEAN bReprojectAxis = TRUE) const;
		FLOAT			getYaw(BOOLEAN bReprojectAxis = TRUE) const;	

		FLOAT			dotProduct(const C3DQuaternion &qtn) const;
		FLOAT			norm() const;

		FLOAT			normalise(); 
		C3DQuaternion	normalisedCopy() const;

		C3DQuaternion	inverse() const;
		C3DQuaternion	unitInverse() const;
		C3DQuaternion	exp() const;
		C3DQuaternion	log() const;

		BOOLEAN			equals(const C3DQuaternion &qtn, FLOAT fTolerance) const;

		BOOLEAN			isNaN() const;

		C3DQuaternion&	operator =(const C3DQuaternion &src);
		BOOLEAN			operator ==(const C3DQuaternion &src) const;
		BOOLEAN			operator !=(const C3DQuaternion &src) const;

		C3DQuaternion	operator +(const C3DQuaternion &src) const;
		C3DQuaternion	operator -(const C3DQuaternion &src) const;
		C3DQuaternion	operator *(const C3DQuaternion &src) const;
		C3DQuaternion	operator *(FLOAT fScalar) const;
		C3DVector		operator *(const C3DVector &vt) const;
		
		C3DQuaternion	operator +() const;
		C3DQuaternion	operator -() const;

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