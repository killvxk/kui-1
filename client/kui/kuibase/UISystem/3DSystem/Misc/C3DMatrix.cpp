#include "OpenHMI.h"

namespace OpenHMI
{
	const C3DMatrix C3DMatrix::ZERO(
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0 );

	const C3DMatrix C3DMatrix::ZEROAFFINE(
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 1 );

	const C3DMatrix C3DMatrix::IDENTITY(
						1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1 );

	const C3DMatrix C3DMatrix::CLIPSPACE2DTOIMAGESPACE(
					  0.5,    0,  0, 0.5, 
						0, -0.5,  0, 0.5, 
						0,    0,  1,   0,
						0,    0,  0,   1);

#define VERTTYPEMUL(op1, op2)		((op1) * (op2))

	C3DMatrix::C3DMatrix()
	{
		m_pf = &m_f[0][0];

		for (INT i = 0; i < 16; i++)
		{
			m_pf[i] = 0.0f;
		}
	}

	C3DMatrix::C3DMatrix(const C3DMatrix &src)
	{
		m_pf = &m_f[0][0];

		for (INT i = 0; i < 16; i++)
		{
			m_pf[i] = src.m_pf[i];
		}
	}

	C3DMatrix::C3DMatrix(	FLOAT f00, FLOAT f01, FLOAT f02, FLOAT f03,
							FLOAT f10, FLOAT f11, FLOAT f12, FLOAT f13,
							FLOAT f20, FLOAT f21, FLOAT f22, FLOAT f23,
							FLOAT f30, FLOAT f31, FLOAT f32, FLOAT f33)
	{
		m_pf = &m_f[0][0];

		m_f[0][0] = f00;
		m_f[0][1] = f01;
		m_f[0][2] = f02;
		m_f[0][3] = f03;
		m_f[1][0] = f10;
		m_f[1][1] = f11;
		m_f[1][2] = f12;
		m_f[1][3] = f13;
		m_f[2][0] = f20;
		m_f[2][1] = f21;
		m_f[2][2] = f22;
		m_f[2][3] = f23;
		m_f[3][0] = f30;
		m_f[3][1] = f31;
		m_f[3][2] = f32;
		m_f[3][3] = f33;
	}

	C3DMatrix::~C3DMatrix()
	{
		;
	}	

	void C3DMatrix::setTrans( const C3DVector &vt)
	{
		m_f[3][0] = vt.getX();
		m_f[3][1] = vt.getY();
		m_f[3][2] = vt.getZ();
	}

	void C3DMatrix::setTrans( FLOAT fX, FLOAT fY, FLOAT fZ)
	{
		m_f[3][0] = fX;
		m_f[3][1] = fY;
		m_f[3][2] = fZ;
	}

	C3DVector C3DMatrix::getTrans() const
	{
		return C3DVector(m_f[0][3], m_f[1][3], m_f[2][3]);
	}

	void C3DMatrix::makeTrans( const C3DVector &vt)
	{
		makeTrans(vt.getX(), vt.getY(), vt.getZ());
	}

	void C3DMatrix::makeTrans( FLOAT fX, FLOAT fY, FLOAT fZ)
	{
		m_f[0][0] = 1.0; m_f[0][1] = 0.0; m_f[0][2] = 0.0; m_f[0][3] = 0.0;
		m_f[1][0] = 0.0; m_f[1][1] = 1.0; m_f[1][2] = 0.0; m_f[1][3] = 0.0;
		m_f[2][0] = 0.0; m_f[2][1] = 0.0; m_f[2][2] = 1.0; m_f[2][3] = 0.0;
		m_f[3][0] = fX;  m_f[3][1] = fY;  m_f[3][2] = fZ;  m_f[3][3] = 1.0;
	}

	void C3DMatrix::setScale( const C3DVector &vt)
	{
		m_f[0][0] = vt.getX();
		m_f[1][1] = vt.getY();
		m_f[2][2] = vt.getZ();
	}

	void C3DMatrix::setScale( FLOAT fX, FLOAT fY, FLOAT fZ)
	{
		m_f[0][0] = fX;
		m_f[1][1] = fY;
		m_f[2][2] = fZ;
	}

	C3DVector C3DMatrix::getScale() const
	{
		return C3DVector(m_f[0][0], m_f[1][1], m_f[2][2]);
	}

	void C3DMatrix::makeScale( const C3DVector &vt)
	{
		makeScale(vt.getX(), vt.getY(), vt.getZ());
	}

	void C3DMatrix::makeScale( FLOAT fX, FLOAT fY, FLOAT fZ)
	{
		m_f[0][0] = fX;  m_f[0][1] = 0.0; m_f[0][2] = 0.0; m_f[0][3] = 0.0;
		m_f[1][0] = 0.0; m_f[1][1] = fY;  m_f[1][2] = 0.0; m_f[1][3] = 0.0;
		m_f[2][0] = 0.0; m_f[2][1] = 0.0; m_f[2][2] = fZ;  m_f[2][3] = 0.0;
		m_f[3][0] = 0.0; m_f[3][1] = 0.0; m_f[3][2] = 0.0; m_f[3][3] = 1.0;
	}

	BOOLEAN C3DMatrix::hasScale() const
	{
		FLOAT fTemp = m_f[0][0] * m_f[0][0] + m_f[1][0] * m_f[1][0] + m_f[2][0] * m_f[2][0];
		if (!Math::isZero(fTemp - 1.0f))
		{
			return TRUE;
		}

		fTemp = m_f[0][1] * m_f[0][1] + m_f[1][1] * m_f[1][1] + m_f[2][1] * m_f[2][1];
		if (!Math::isZero(fTemp - 1.0f))
		{
			return TRUE;
		}

		fTemp = m_f[0][2] * m_f[0][2] + m_f[1][2] * m_f[1][2] + m_f[2][2] * m_f[2][2];
		if (!Math::isZero(fTemp - 1.0f))
		{
			return TRUE;
		}

		return FALSE;
	}

	BOOLEAN C3DMatrix::hasNegativeScale() const
	{
		return determinant() < 0;
	}

	void C3DMatrix::rotateX(const FLOAT fAngle)
	{
		FLOAT fCosine, fSine;

		fCosine = (FLOAT)Math::cos(fAngle);
		fSine   = (FLOAT)Math::sin(fAngle);

		/* Create the trigonometric matrix corresponding to X Rotation */
		m_f[0][0] = 1.0f;		m_f[1][0] = 0.0f;		m_f[2][0] = 0.0f;		m_f[3][0] = 0.0f;
		m_f[0][1] = 0.0f;	    m_f[1][1] = fCosine;	m_f[2][1] = fSine;		m_f[3][1] = 0.0f;
		m_f[0][2] = 0.0f;		m_f[1][2] = -fSine;		m_f[2][2] = fCosine;	m_f[3][2] = 0.0f;
		m_f[0][3] = 0.0f;		m_f[1][3] = 0.0f;		m_f[2][3] = 0.0f;		m_f[3][3] = 1.0f;
	}

	void C3DMatrix::rotateY(const FLOAT fAngle)
	{
		FLOAT fCosine, fSine;
		
		fCosine = (FLOAT)Math::cos(fAngle);
		fSine   = (FLOAT)Math::sin(fAngle);

		/* Create the trigonometric matrix corresponding to Y Rotation */
		m_f[0][0] = fCosine;	m_f[1][0] = 0.0f;		m_f[2][0] = -fSine;		m_f[3][0] = 0.0f;
		m_f[0][1] = 0.0f;	    m_f[1][1] = 1.0f;		m_f[2][1] = 0.0f;		m_f[3][1] = 0.0f;
		m_f[0][2] = fSine;		m_f[1][2] = 0.0f;		m_f[2][2] = fCosine;	m_f[3][2] = 0.0f;
		m_f[0][3] = 0.0f;		m_f[1][3] = 0.0f;		m_f[2][3] = 0.0f;		m_f[3][3] = 1.0f;
	}

	void C3DMatrix::rotateZ(const FLOAT fAngle)
	{
		FLOAT fCosine, fSine;
		
		fCosine = (FLOAT)Math::cos(fAngle);
		fSine   = (FLOAT)Math::sin(fAngle);

		/* Create the trigonometric matrix corresponding to Z Rotation */
		m_f[0][0] = fCosine;	m_f[1][0] = fSine;		m_f[2][0] = 0.0f;		m_f[3][0] = 0.0f;
		m_f[0][1] = -fSine;	    m_f[1][1] = fCosine;    m_f[2][1] = 0.0f;		m_f[3][1] = 0.0f;
		m_f[0][2] = 0.0f;		m_f[1][2] = 0.0f;		m_f[2][2] = 1.0f;		m_f[3][2] = 0.0f;
		m_f[0][3] = 0.0f;		m_f[1][3] = 0.0f;		m_f[2][3] = 0.0f;		m_f[3][3] = 1.0f;
	}


	C3DQuaternion C3DMatrix::extractQuaternion() const
	{
		return C3DQuaternion::ZERO;
	}

	BOOLEAN C3DMatrix::isAffine() const
	{
		return m_f[3][0] == 0 && m_f[3][1] == 0 && m_f[3][2] == 0 && m_f[3][3] == 1;
	}

	void C3DMatrix::makeLookAt(const C3DVector &eye, const C3DVector &target, const C3DVector &up)
	{
		C3DVector xAxis;
		C3DVector yAxis;
		C3DVector zAxis = eye - target;

		zAxis.normalise();

		xAxis = up.crossProduct(zAxis);
		xAxis.normalise();

		yAxis = zAxis.crossProduct(xAxis);
		yAxis.normalise();
	 
		m_f[0][0] = xAxis.getX();
		m_f[1][0] = xAxis.getY();
		m_f[2][0] = xAxis.getZ();
		m_f[3][0] = -xAxis.dotProduct(eye);

		m_f[0][1] = yAxis.getX();
		m_f[1][1] = yAxis.getY();
		m_f[2][1] = yAxis.getZ();
		m_f[3][1] = -yAxis.dotProduct(eye);

		m_f[0][2] = zAxis.getX();
		m_f[1][2] = zAxis.getY();
		m_f[2][2] = zAxis.getZ();    
		m_f[3][2] = -zAxis.dotProduct(eye);

		m_f[0][3] = 0.0f;
		m_f[1][3] = 0.0f;
		m_f[2][3] = 0.0f;
		m_f[3][3] = 1.0f;
	}

	void C3DMatrix::makePerspective(FLOAT fovx, FLOAT aspect, FLOAT znear, FLOAT zfar)
	{
		FLOAT e = 1.0f / tanf( Math::toRadian(fovx) / 2.0f);
		FLOAT aspectInv = 1.0f / aspect;
		FLOAT fovy = 2.0f * atanf(aspectInv / e);
		FLOAT xScale = 1.0f / tanf(0.5f * fovy);
		FLOAT yScale = xScale / aspectInv;

		m_f[0][0] = xScale;
		m_f[0][1] = 0.0f;
		m_f[0][2] = 0.0f;
		m_f[0][3] = 0.0f;

		m_f[1][0] = 0.0f;
		m_f[1][1] = yScale;
		m_f[1][2] = 0.0f;
		m_f[1][3] = 0.0f;

		m_f[2][0] = 0.0f;
		m_f[2][1] = 0.0f;
		m_f[2][2] = (zfar + znear) / (znear - zfar);
		m_f[2][3] = -1.0f;

		m_f[3][0] = 0.0f;
		m_f[3][1] = 0.0f;
		m_f[3][2] = (2.0f * zfar * znear) / (znear - zfar);
		m_f[3][3] = 0.0f;
	}

	void C3DMatrix::makePerspective(FLOAT l, FLOAT r, FLOAT t, FLOAT b, FLOAT n, FLOAT f)
	{
		m_f[0][0] = 2.0f * n/(r-l);
		m_f[0][1] = 0.0f;
		m_f[0][2] = 0.0f;
		m_f[0][3] = 0.0f;

		m_f[1][0] = 0.0f;
		m_f[1][1] = 2.0f * n/(t-b);
		m_f[1][2] = 0.0f;
		m_f[1][3] = 0.0f;

		m_f[2][0] = 0.0f;
		m_f[2][1] = 0.0f;
		m_f[2][2] = (f + n) / (n - f);
		m_f[2][3] = -1.0f;

		m_f[3][0] = 0.0f;
		m_f[3][1] = 0.0f;
		m_f[3][2] = (2.0f * f * n) / (n - f);
		m_f[3][3] = 0.0f;
	}

	void C3DMatrix::makeOrthognal(FLOAT l, FLOAT r, FLOAT t, FLOAT b, FLOAT n, FLOAT f)
	{
		m_f[0][0] = 2.0f / (r - l);
		m_f[0][1] = 0.0f;
		m_f[0][2] = 0.0f;
		m_f[0][3] = 0.0f;

		m_f[1][0] = 0.0f;
		m_f[1][1] = 2.0f / (t - b);
		m_f[1][2] = 0.0f;
		m_f[1][3] = 0.0f;

		m_f[2][0] = 0.0f;
		m_f[2][1] = 0.0f;
		m_f[2][2] = 2.0f / (f - n);
		m_f[2][3] = 0.0f;

		
		m_f[3][0] = -(r + l) / (r - l);
		m_f[3][1] = -(t + b) / (t - b);
		m_f[3][2] =  -(f + n) / (f - n);
		m_f[3][3] = 1.0f;
	}

	C3DMatrix C3DMatrix::concatenate(const C3DMatrix &mtx2) const
	{
		C3DMatrix mtx;

		mtx.m_f[0][0] = mtx2.m_f[0][0] * m_f[0][0] + mtx2.m_f[0][1] * m_f[1][0] + mtx2.m_f[0][2] * m_f[2][0] + mtx2.m_f[0][3] * m_f[3][0];
		mtx.m_f[0][1] = mtx2.m_f[0][0] * m_f[0][1] + mtx2.m_f[0][1] * m_f[1][1] + mtx2.m_f[0][2] * m_f[2][1] + mtx2.m_f[0][3] * m_f[3][1];
		mtx.m_f[0][2] = mtx2.m_f[0][0] * m_f[0][2] + mtx2.m_f[0][1] * m_f[1][2] + mtx2.m_f[0][2] * m_f[2][2] + mtx2.m_f[0][3] * m_f[3][2];
		mtx.m_f[0][3] = mtx2.m_f[0][0] * m_f[0][3] + mtx2.m_f[0][1] * m_f[1][3] + mtx2.m_f[0][2] * m_f[2][3] + mtx2.m_f[0][3] * m_f[3][3];

		mtx.m_f[1][0] = mtx2.m_f[1][0] * m_f[0][0] + mtx2.m_f[1][1] * m_f[1][0] + mtx2.m_f[1][2] * m_f[2][0] + mtx2.m_f[1][3] * m_f[3][0];
		mtx.m_f[1][1] = mtx2.m_f[1][0] * m_f[0][1] + mtx2.m_f[1][1] * m_f[1][1] + mtx2.m_f[1][2] * m_f[2][1] + mtx2.m_f[1][3] * m_f[3][1];
		mtx.m_f[1][2] = mtx2.m_f[1][0] * m_f[0][2] + mtx2.m_f[1][1] * m_f[1][2] + mtx2.m_f[1][2] * m_f[2][2] + mtx2.m_f[1][3] * m_f[3][2];
		mtx.m_f[1][3] = mtx2.m_f[1][0] * m_f[0][3] + mtx2.m_f[1][1] * m_f[1][3] + mtx2.m_f[1][2] * m_f[2][3] + mtx2.m_f[1][3] * m_f[3][3];

		mtx.m_f[2][0] = mtx2.m_f[2][0] * m_f[0][0] + mtx2.m_f[2][1] * m_f[1][0] + mtx2.m_f[2][2] * m_f[2][0] + mtx2.m_f[2][3] * m_f[3][0];
		mtx.m_f[2][1] = mtx2.m_f[2][0] * m_f[0][1] + mtx2.m_f[2][1] * m_f[1][1] + mtx2.m_f[2][2] * m_f[2][1] + mtx2.m_f[2][3] * m_f[3][1];
		mtx.m_f[2][2] = mtx2.m_f[2][0] * m_f[0][2] + mtx2.m_f[2][1] * m_f[1][2] + mtx2.m_f[2][2] * m_f[2][2] + mtx2.m_f[2][3] * m_f[3][2];
		mtx.m_f[2][3] = mtx2.m_f[2][0] * m_f[0][3] + mtx2.m_f[2][1] * m_f[1][3] + mtx2.m_f[2][2] * m_f[2][3] + mtx2.m_f[2][3] * m_f[3][3];

		mtx.m_f[3][0] = mtx2.m_f[3][0] * m_f[0][0] + mtx2.m_f[3][1] * m_f[1][0] + mtx2.m_f[3][2] * m_f[2][0] + mtx2.m_f[3][3] * m_f[3][0];
		mtx.m_f[3][1] = mtx2.m_f[3][0] * m_f[0][1] + mtx2.m_f[3][1] * m_f[1][1] + mtx2.m_f[3][2] * m_f[2][1] + mtx2.m_f[3][3] * m_f[3][1];
		mtx.m_f[3][2] = mtx2.m_f[3][0] * m_f[0][2] + mtx2.m_f[3][1] * m_f[1][2] + mtx2.m_f[3][2] * m_f[2][2] + mtx2.m_f[3][3] * m_f[3][2];
		mtx.m_f[3][3] = mtx2.m_f[3][0] * m_f[0][3] + mtx2.m_f[3][1] * m_f[1][3] + mtx2.m_f[3][2] * m_f[2][3] + mtx2.m_f[3][3] * m_f[3][3];

		return mtx;
	}

	C3DMatrix C3DMatrix::transpose() const
	{
		return C3DMatrix(	m_f[0][0], m_f[1][0], m_f[2][0], m_f[3][0],
							m_f[0][1], m_f[1][1], m_f[2][1], m_f[3][1],
							m_f[0][2], m_f[1][2], m_f[2][2], m_f[3][2],
							m_f[0][3], m_f[1][3], m_f[2][3], m_f[3][3]);
	}	

	C3DMatrix C3DMatrix::adjoint() const
	{
		return C3DMatrix(	myMinor(1, 2, 3, 1, 2, 3),
							-myMinor(0, 2, 3, 1, 2, 3),
							myMinor(0, 1, 3, 1, 2, 3),
							-myMinor(0, 1, 2, 1, 2, 3),

							-myMinor(1, 2, 3, 0, 2, 3),
							myMinor(0, 2, 3, 0, 2, 3),
							-myMinor(0, 1, 3, 0, 2, 3),
							myMinor(0, 1, 2, 0, 2, 3),

							myMinor(1, 2, 3, 0, 1, 3),
							-myMinor(0, 2, 3, 0, 1, 3),
							myMinor(0, 1, 3, 0, 1, 3),
							-myMinor(0, 1, 2, 0, 1, 3),

							-myMinor(1, 2, 3, 0, 1, 2),
							myMinor(0, 2, 3, 0, 1, 2),
							-myMinor(0, 1, 3, 0, 1, 2),
							myMinor(0, 1, 2, 0, 1, 2)   );
	}

	FLOAT C3DMatrix::determinant() const
	{
		return	m_f[0][0] * myMinor(1, 2, 3, 1, 2, 3) -
				m_f[0][1] * myMinor(1, 2, 3, 0, 2, 3) +
				m_f[0][2] * myMinor(1, 2, 3, 0, 1, 3) -
				m_f[0][3] * myMinor(1, 2, 3, 0, 1, 2);
	}

	C3DMatrix C3DMatrix::inverse() const
	{
		FLOAT m00 = m_f[0][0], m01 = m_f[0][1], m02 = m_f[0][2], m03 = m_f[0][3];
		FLOAT m10 = m_f[1][0], m11 = m_f[1][1], m12 = m_f[1][2], m13 = m_f[1][3];
		FLOAT m20 = m_f[2][0], m21 = m_f[2][1], m22 = m_f[2][2], m23 = m_f[2][3];
		FLOAT m30 = m_f[3][0], m31 = m_f[3][1], m32 = m_f[3][2], m33 = m_f[3][3];

		FLOAT v0 = m20 * m31 - m21 * m30;
		FLOAT v1 = m20 * m32 - m22 * m30;
		FLOAT v2 = m20 * m33 - m23 * m30;
		FLOAT v3 = m21 * m32 - m22 * m31;
		FLOAT v4 = m21 * m33 - m23 * m31;
		FLOAT v5 = m22 * m33 - m23 * m32;

		FLOAT t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
		FLOAT t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
		FLOAT t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
		FLOAT t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

		FLOAT invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

		FLOAT d00 = t00 * invDet;
		FLOAT d10 = t10 * invDet;
		FLOAT d20 = t20 * invDet;
		FLOAT d30 = t30 * invDet;

		FLOAT d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		FLOAT d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		FLOAT d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		FLOAT d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m10 * m31 - m11 * m30;
		v1 = m10 * m32 - m12 * m30;
		v2 = m10 * m33 - m13 * m30;
		v3 = m11 * m32 - m12 * m31;
		v4 = m11 * m33 - m13 * m31;
		v5 = m12 * m33 - m13 * m32;

		FLOAT d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		FLOAT d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		FLOAT d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		FLOAT d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m21 * m10 - m20 * m11;
		v1 = m22 * m10 - m20 * m12;
		v2 = m23 * m10 - m20 * m13;
		v3 = m22 * m11 - m21 * m12;
		v4 = m23 * m11 - m21 * m13;
		v5 = m23 * m12 - m22 * m13;

		FLOAT d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		FLOAT d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		FLOAT d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		FLOAT d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		return C3DMatrix(	d00, d01, d02, d03,
							d10, d11, d12, d13,
							d20, d21, d22, d23,
							d30, d31, d32, d33);
	}

	C3DMatrix C3DMatrix::inverseAffine() const
	{
		if (!isAffine())
		{
			return C3DMatrix();
		}

		FLOAT m10 = m_f[1][0], m11 = m_f[1][1], m12 = m_f[1][2];
		FLOAT m20 = m_f[2][0], m21 = m_f[2][1], m22 = m_f[2][2];

		FLOAT t00 = m22 * m11 - m21 * m12;
		FLOAT t10 = m20 * m12 - m22 * m10;
		FLOAT t20 = m21 * m10 - m20 * m11;

		FLOAT m00 = m_f[0][0], m01 = m_f[0][1], m02 = m_f[0][2];

		FLOAT invDet = 1 / (m00 * t00 + m01 * t10 + m02 * t20);

		t00 *= invDet; t10 *= invDet; t20 *= invDet;

		m00 *= invDet; m01 *= invDet; m02 *= invDet;

		FLOAT r00 = t00;
		FLOAT r01 = m02 * m21 - m01 * m22;
		FLOAT r02 = m01 * m12 - m02 * m11;

		FLOAT r10 = t10;
		FLOAT r11 = m00 * m22 - m02 * m20;
		FLOAT r12 = m02 * m10 - m00 * m12;

		FLOAT r20 = t20;
		FLOAT r21 = m01 * m20 - m00 * m21;
		FLOAT r22 = m00 * m11 - m01 * m10;

		FLOAT m03 = m_f[0][3], m13 = m_f[1][3], m23 = m_f[2][3];

		FLOAT r03 = - (r00 * m03 + r01 * m13 + r02 * m23);
		FLOAT r13 = - (r10 * m03 + r11 * m13 + r12 * m23);
		FLOAT r23 = - (r20 * m03 + r21 * m13 + r22 * m23);

		return C3DMatrix(	r00, r01, r02, r03,
							r10, r11, r12, r13,
							r20, r21, r22, r23,
							0,   0,   0,   1);
	}

	void C3DMatrix::identity()
	{
		*this = C3DMatrix::IDENTITY;
	}

	void C3DMatrix::makeTransform(const C3DVector &vtPosition, const C3DVector &vtScale, const C3DQuaternion &qtnOrientation)
	{
		;
	}

	void C3DMatrix::makeInverseTransform(const C3DVector& position, const C3DVector& scale, const C3DQuaternion &qtnOrientation)
	{
		;
	}

	void C3DMatrix::decomposition(C3DVector& position, C3DVector& scale, C3DQuaternion &qtnOrientation) const
	{
		;
	}

	C3DMatrix C3DMatrix::concatenateAffine(const C3DMatrix &mtx2) const
	{
		if (!isAffine() || !mtx2.isAffine())
		{
			return C3DMatrix();
		}

		return C3DMatrix(
					m_f[0][0] * mtx2.m_f[0][0] + m_f[0][1] * mtx2.m_f[1][0] + m_f[0][2] * mtx2.m_f[2][0],
					m_f[0][0] * mtx2.m_f[0][1] + m_f[0][1] * mtx2.m_f[1][1] + m_f[0][2] * mtx2.m_f[2][1],
					m_f[0][0] * mtx2.m_f[0][2] + m_f[0][1] * mtx2.m_f[1][2] + m_f[0][2] * mtx2.m_f[2][2],
					m_f[0][0] * mtx2.m_f[0][3] + m_f[0][1] * mtx2.m_f[1][3] + m_f[0][2] * mtx2.m_f[2][3] + m_f[0][3],

					m_f[1][0] * mtx2.m_f[0][0] + m_f[1][1] * mtx2.m_f[1][0] + m_f[1][2] * mtx2.m_f[2][0],
					m_f[1][0] * mtx2.m_f[0][1] + m_f[1][1] * mtx2.m_f[1][1] + m_f[1][2] * mtx2.m_f[2][1],
					m_f[1][0] * mtx2.m_f[0][2] + m_f[1][1] * mtx2.m_f[1][2] + m_f[1][2] * mtx2.m_f[2][2],
					m_f[1][0] * mtx2.m_f[0][3] + m_f[1][1] * mtx2.m_f[1][3] + m_f[1][2] * mtx2.m_f[2][3] + m_f[1][3],

					m_f[2][0] * mtx2.m_f[0][0] + m_f[2][1] * mtx2.m_f[1][0] + m_f[2][2] * mtx2.m_f[2][0],
					m_f[2][0] * mtx2.m_f[0][1] + m_f[2][1] * mtx2.m_f[1][1] + m_f[2][2] * mtx2.m_f[2][1],
					m_f[2][0] * mtx2.m_f[0][2] + m_f[2][1] * mtx2.m_f[1][2] + m_f[2][2] * mtx2.m_f[2][2],
					m_f[2][0] * mtx2.m_f[0][3] + m_f[2][1] * mtx2.m_f[1][3] + m_f[2][2] * mtx2.m_f[2][3] + m_f[2][3],

					0, 0, 0, 1);
	}

	C3DVector C3DMatrix::transformAffine(const C3DVector &vt) const
	{
		if (!isAffine())
		{
			return C3DVector::ZERO;
		}

		return C3DVector(	m_f[0][0] * vt.getX() + m_f[0][1] * vt.getY() + m_f[0][2] * vt.getZ() + m_f[0][3], 
							m_f[1][0] * vt.getX() + m_f[1][1] * vt.getY() + m_f[1][2] * vt.getZ() + m_f[1][3],
							m_f[2][0] * vt.getX() + m_f[2][1] * vt.getY() + m_f[2][2] * vt.getZ() + m_f[2][3]);
	}

	C3DMatrix& C3DMatrix::operator =(const C3DMatrix &src)
	{
		for (INT i = 0; i < 16; i++)
		{
			m_pf[i] = src.m_pf[i];
		}

		return *this;
	}

	BOOLEAN C3DMatrix::operator ==(const C3DMatrix &src) const
	{
		for (INT i = 0; i < 16; i++)
		{
			if (!Math::isZero(m_pf[i] - src.m_pf[i]))
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	BOOLEAN C3DMatrix::operator !=(const C3DMatrix &src) const
	{
		return	!operator==(src);
	}

	C3DMatrix C3DMatrix::operator +(const C3DMatrix &src) const
	{
		C3DMatrix mtx;

		for (INT i = 0; i < 16; i++)
		{
			mtx.m_pf[i] = m_pf[i] + src.m_pf[i];
		}

		return mtx;
	}

	C3DMatrix C3DMatrix::operator -(const C3DMatrix &src) const
	{
		C3DMatrix mtx;

		for (INT i = 0; i < 16; i++)
		{
			mtx.m_pf[i] = m_pf[i] - src.m_pf[i];
		}

		return mtx;
	}

	C3DMatrix C3DMatrix::operator *(FLOAT fScalar) const
	{
		C3DMatrix mtx;

		for (INT i = 0; i < 16; i++)
		{
			mtx.m_pf[i] = m_pf[i] * fScalar;
		}

		return mtx;
	}

	C3DMatrix C3DMatrix::operator *(const C3DMatrix &src) const
	{
		return concatenate(src);
	}

	C3DVector C3DMatrix::operator *(const C3DVector &vt) const
	{
		C3DVector vtR;

		FLOAT fInvW = 1.0f / ( m_f[3][0] * vt.getX() + m_f[3][1] * vt.getY() + m_f[3][2] * vt.getZ() + m_f[3][3] );

		vtR.setX( ( m_f[0][0] * vt.getX() + m_f[1][0] * vt.getY() + m_f[2][0] * vt.getZ() + m_f[0][3] ) * fInvW );
		vtR.setY( ( m_f[0][1] * vt.getX() + m_f[1][1] * vt.getY() + m_f[2][1] * vt.getZ() + m_f[1][3] ) * fInvW );
		vtR.setZ( ( m_f[0][2] * vt.getX() + m_f[1][2] * vt.getY() + m_f[2][2] * vt.getZ() + m_f[2][3] ) * fInvW );

		return vtR;
	}

	C3DVector4 C3DMatrix::operator *(const C3DVector4 &vt) const
	{
		C3DVector4 vtR;

		vtR.setX( ( m_f[0][0] * vt.getX() + m_f[1][0] * vt.getY() + m_f[2][0] * vt.getZ() + m_f[3][0] * vt.getW() ) );
		vtR.setY( ( m_f[0][1] * vt.getX() + m_f[1][1] * vt.getY() + m_f[2][1] * vt.getZ() + m_f[3][1] * vt.getW() ) );
		vtR.setZ( ( m_f[0][2] * vt.getX() + m_f[1][2] * vt.getY() + m_f[2][2] * vt.getZ() + m_f[3][2] * vt.getW() ) );
		vtR.setW( ( m_f[0][3] * vt.getX() + m_f[1][3] * vt.getY() + m_f[2][3] * vt.getZ() + m_f[3][3] * vt.getW() ) );

		return vtR;
	}

	C3DMatrix& C3DMatrix::operator*=(const C3DMatrix &src)
	{
		C3DMatrix result;
		FLOAT* pf = &m_f[0][0];
		const FLOAT* rhspf = src.ptr();
		// col 0
		result.m_f[0][0] =	VERTTYPEMUL(pf[0],rhspf[0])+VERTTYPEMUL(pf[4],rhspf[1])+VERTTYPEMUL(pf[8],rhspf[2])+VERTTYPEMUL(pf[12],rhspf[3]);
		result.m_f[0][1] =	VERTTYPEMUL(pf[1],rhspf[0])+VERTTYPEMUL(pf[5],rhspf[1])+VERTTYPEMUL(pf[9],rhspf[2])+VERTTYPEMUL(pf[13],rhspf[3]);
		result.m_f[0][2] =	VERTTYPEMUL(pf[2],rhspf[0])+VERTTYPEMUL(pf[6],rhspf[1])+VERTTYPEMUL(pf[10],rhspf[2])+VERTTYPEMUL(pf[14],rhspf[3]);
		result.m_f[0][3] =	VERTTYPEMUL(pf[3],rhspf[0])+VERTTYPEMUL(pf[7],rhspf[1])+VERTTYPEMUL(pf[11],rhspf[2])+VERTTYPEMUL(pf[15],rhspf[3]);

		// col 1
		result.m_f[1][0] =	VERTTYPEMUL(pf[0],rhspf[4])+VERTTYPEMUL(pf[4],rhspf[5])+VERTTYPEMUL(pf[8],rhspf[6])+VERTTYPEMUL(pf[12],rhspf[7]);
		result.m_f[1][1] =	VERTTYPEMUL(pf[1],rhspf[4])+VERTTYPEMUL(pf[5],rhspf[5])+VERTTYPEMUL(pf[9],rhspf[6])+VERTTYPEMUL(pf[13],rhspf[7]);
		result.m_f[1][2] =	VERTTYPEMUL(pf[2],rhspf[4])+VERTTYPEMUL(pf[6],rhspf[5])+VERTTYPEMUL(pf[10],rhspf[6])+VERTTYPEMUL(pf[14],rhspf[7]);
		result.m_f[1][3] =	VERTTYPEMUL(pf[3],rhspf[4])+VERTTYPEMUL(pf[7],rhspf[5])+VERTTYPEMUL(pf[11],rhspf[6])+VERTTYPEMUL(pf[15],rhspf[7]);

		// col 2
		result.m_f[2][0] =	VERTTYPEMUL(pf[0],rhspf[8])+VERTTYPEMUL(pf[4],rhspf[9])+VERTTYPEMUL(pf[8],rhspf[10])+VERTTYPEMUL(pf[12],rhspf[11]);
		result.m_f[2][1] =	VERTTYPEMUL(pf[1],rhspf[8])+VERTTYPEMUL(pf[5],rhspf[9])+VERTTYPEMUL(pf[9],rhspf[10])+VERTTYPEMUL(pf[13],rhspf[11]);
		result.m_f[2][2] =	VERTTYPEMUL(pf[2],rhspf[8])+VERTTYPEMUL(pf[6],rhspf[9])+VERTTYPEMUL(pf[10],rhspf[10])+VERTTYPEMUL(pf[14],rhspf[11]);
		result.m_f[2][3] =	VERTTYPEMUL(pf[3],rhspf[8])+VERTTYPEMUL(pf[7],rhspf[9])+VERTTYPEMUL(pf[11],rhspf[10])+VERTTYPEMUL(pf[15],rhspf[11]);

		// col 3
		result.m_f[3][0] =	VERTTYPEMUL(pf[0],rhspf[12])+VERTTYPEMUL(pf[4],rhspf[13])+VERTTYPEMUL(pf[8],rhspf[14])+VERTTYPEMUL(pf[12],rhspf[15]);
		result.m_f[3][1] =	VERTTYPEMUL(pf[1],rhspf[12])+VERTTYPEMUL(pf[5],rhspf[13])+VERTTYPEMUL(pf[9],rhspf[14])+VERTTYPEMUL(pf[13],rhspf[15]);
		result.m_f[3][2] =	VERTTYPEMUL(pf[2],rhspf[12])+VERTTYPEMUL(pf[6],rhspf[13])+VERTTYPEMUL(pf[10],rhspf[14])+VERTTYPEMUL(pf[14],rhspf[15]);
		result.m_f[3][3] =	VERTTYPEMUL(pf[3],rhspf[12])+VERTTYPEMUL(pf[7],rhspf[13])+VERTTYPEMUL(pf[11],rhspf[14])+VERTTYPEMUL(pf[15],rhspf[15]);

		*this = result;
		return *this;
	}

	FLOAT C3DMatrix::myMinor(BYTE r0, BYTE r1, BYTE r2, BYTE c0, BYTE c1, BYTE c2) const
	{
		return	m_f[r0][c0] * (m_f[r1][c1] * m_f[r2][c2] - m_f[r2][c1] * m_f[r1][c2]) -
				m_f[r0][c1] * (m_f[r1][c0] * m_f[r2][c2] - m_f[r2][c0] * m_f[r1][c2]) +
				m_f[r0][c2] * (m_f[r1][c0] * m_f[r2][c1] - m_f[r2][c0] * m_f[r1][c1]);
	}
}