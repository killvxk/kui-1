/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API C3DMatrix : public Object
	{
	public:// const define	
		static	const	C3DMatrix	ZERO;
		static	const	C3DMatrix	ZEROAFFINE;
		static	const	C3DMatrix	IDENTITY;
        static	const	C3DMatrix	CLIPSPACE2DTOIMAGESPACE;

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		C3DMatrix();
		C3DMatrix(const C3DMatrix &src);
		C3DMatrix(	FLOAT f00, FLOAT f01, FLOAT f02, FLOAT f03,
					FLOAT f10, FLOAT f11, FLOAT f12, FLOAT f13,
					FLOAT f20, FLOAT f21, FLOAT f22, FLOAT f23,
					FLOAT f30, FLOAT f31, FLOAT f32, FLOAT f33);
		virtual ~C3DMatrix();

		void			setTrans( const C3DVector &vt);
		void			setTrans( FLOAT fX, FLOAT fY, FLOAT fZ);
		C3DVector		getTrans() const;
		void			makeTrans( const C3DVector &vt);
		void			makeTrans( FLOAT fX, FLOAT fY, FLOAT fZ);

		void			setScale( const C3DVector &vt);
		void			setScale( FLOAT fX, FLOAT fY, FLOAT fZ);
		C3DVector		getScale() const;
		void			makeScale( const C3DVector &vt);
		void			makeScale( FLOAT fX, FLOAT fY, FLOAT fZ);
		BOOLEAN			hasScale() const;
		BOOLEAN			hasNegativeScale() const;

		void	        rotateX(const FLOAT fAngle);
		void            rotateY(const FLOAT fAngle);
		void     		rotateZ(const FLOAT fAngle);

		C3DQuaternion	extractQuaternion() const;

		BOOLEAN			isAffine() const;

		void			makeLookAt(const C3DVector &eye, const C3DVector &target, const C3DVector &up);
		void			makePerspective(FLOAT fovx, FLOAT aspect, FLOAT znear, FLOAT zfar);
		void			makePerspective(FLOAT l, FLOAT r, FLOAT t, FLOAT b, FLOAT n, FLOAT f);
		void			makeOrthognal(FLOAT l, FLOAT r, FLOAT t, FLOAT b, FLOAT n, FLOAT f);

		C3DMatrix		concatenate(const C3DMatrix &mtx2) const;
		C3DMatrix		transpose() const;		
		C3DMatrix		adjoint() const;
		FLOAT			determinant() const;
		C3DMatrix		inverse() const;
		C3DMatrix		inverseAffine() const;

		void			identity();

		void			makeTransform(const C3DVector &vtPosition, const C3DVector &vtScale, const C3DQuaternion &qtnOrientation);
		void			makeInverseTransform(const C3DVector& position, const C3DVector& scale, const C3DQuaternion &qtnOrientation);
		void			decomposition(C3DVector& position, C3DVector& scale, C3DQuaternion &qtnOrientation) const;
		C3DMatrix		concatenateAffine(const C3DMatrix &mtx2) const;
		C3DVector		transformAffine(const C3DVector &vt) const;

		C3DMatrix&	operator =(const C3DMatrix &src);
		BOOLEAN		operator ==(const C3DMatrix &src) const;
		BOOLEAN		operator !=(const C3DMatrix &src) const;

		C3DMatrix	operator +(const C3DMatrix &src) const;
		C3DMatrix	operator -(const C3DMatrix &src) const;
		C3DMatrix	operator *(FLOAT fScalar) const;
		C3DMatrix	operator *(const C3DMatrix &src) const;
		C3DVector	operator *(const C3DVector &vt) const;
		C3DVector4	operator *(const C3DVector4 &vt) const;
		C3DMatrix&	operator*=(const C3DMatrix &src);

		FLOAT*			operator[](UINT row);
		const FLOAT*	operator[](UINT row) const;
		
		FLOAT*		ptr() const	{ return m_pf;	}

	protected:// method

	private:// method
		FLOAT	myMinor(BYTE r0, BYTE r1, BYTE r2, BYTE c0, BYTE c1, BYTE c2) const;

	protected:// property

	private:// property
		FLOAT	m_f[4][4];
		FLOAT	*m_pf;
	};

	inline FLOAT *C3DMatrix::operator[](UINT row)
	{
		return m_f[row];
	}

	inline const FLOAT *C3DMatrix::operator[](UINT row) const
	{
		return m_f[row];
	}
}