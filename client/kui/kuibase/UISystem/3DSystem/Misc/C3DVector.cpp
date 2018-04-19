#include "OpenHMI.h"

namespace OpenHMI
{
    const	C3DVector	C3DVector::ZERO( 0.0f, 0.0f, 0.0f );
    const	C3DVector	C3DVector::UNIT_X( 1.0f, 0.0f, 0.0f );
    const	C3DVector	C3DVector::UNIT_Y( 0.0f, 1.0f, 0.0f );
    const	C3DVector	C3DVector::UNIT_Z( 0.0f, 0.0f, 1.0f );
    const	C3DVector	C3DVector::NEGATIVE_UNIT_X( -1.0f, 0.0f, 0.0f );
    const	C3DVector	C3DVector::NEGATIVE_UNIT_Y( 0.0f, -1.0f, 0.0f );
    const	C3DVector	C3DVector::NEGATIVE_UNIT_Z( 0.0f,  0.0f, -1.0f );
    const	C3DVector	C3DVector::UNIT_SCALE( 1.0f, 1.0f, 1.0f );

	C3DVector::C3DVector()
		: m_fX(0.0f)
		, m_fY(0.0f)
		, m_fZ(0.0f)
	{
		;
	}

	C3DVector::C3DVector(const C3DVector &src) 
		: m_fX(src.m_fX)
		, m_fY(src.m_fY)
		, m_fZ(src.m_fZ)
	{
		;
	}

	C3DVector::C3DVector(FLOAT fX, FLOAT fY, FLOAT fZ)
		: m_fX(fX)
		, m_fY(fY)
		, m_fZ(fZ)
	{
		;
	}

	C3DVector::~C3DVector()
	{
		;
	}

	FLOAT C3DVector::length() const
	{
		return (FLOAT)Math::sqrt( m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ );
	}

	FLOAT C3DVector::squaredLength() const
	{
		return m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ;
	}

	BOOLEAN C3DVector::isZeroLength() const
	{
		return Math::isZero(squaredLength());
	}

	FLOAT C3DVector::distance(const C3DVector& vt) const
	{
		return (*this - vt).length();
	}

	FLOAT C3DVector::squaredDistance(const C3DVector& vt) const
	{
		return (*this - vt).squaredLength();
	}

	FLOAT C3DVector::normalise()
	{
		FLOAT fLength = length();

		if ( fLength > 0.0f )
		{
			FLOAT fInvLength = 1.0f / fLength;
			m_fX *= fInvLength;
			m_fY *= fInvLength;
			m_fZ *= fInvLength;
		}

		return fLength;
	}

	C3DVector C3DVector::normalisedCopy() const
	{
		C3DVector vt = *this;
		vt.normalise();
		return vt;
	}

	FLOAT C3DVector::dotProduct(const C3DVector& vt) const
	{
		 return m_fX * vt.m_fX + m_fY * vt.m_fY + m_fZ * vt.m_fZ;
	}

	FLOAT C3DVector::absDotProduct(const C3DVector& vt) const
	{
		return Math::abs(m_fX * vt.m_fX) + Math::abs(m_fY * vt.m_fY) +Math::abs( m_fZ * vt.m_fZ);
	}

	C3DVector C3DVector::crossProduct(const C3DVector& vt) const
	{
		return C3DVector(	m_fY * vt.m_fZ - m_fZ * vt.m_fY,
							m_fZ * vt.m_fX - m_fX * vt.m_fZ,
							m_fX * vt.m_fY - m_fY * vt.m_fX   );
	}

	C3DVector C3DVector::midPoint(const C3DVector &vt) const
	{
		return C3DVector(	( m_fX + vt.m_fX ) * 0.5f,
							( m_fY + vt.m_fY ) * 0.5f,
							( m_fZ + vt.m_fZ ) * 0.5f );
	}

	void C3DVector::makeFloor(const C3DVector &vt)
	{
		if ( vt.m_fX < m_fX )
		{
			m_fX = vt.m_fX;
		}

		if ( vt.m_fY < m_fY )
		{
			m_fY = vt.m_fY;
		}

		if ( vt.m_fZ < m_fZ )
		{
			m_fZ = vt.m_fZ;
		}
	}

	void C3DVector::makeCeil(const C3DVector &vt)
	{
		if ( vt.m_fX > m_fX )
		{
			m_fX = vt.m_fX;
		}

		if ( vt.m_fY > m_fY )
		{
			m_fY = vt.m_fY;
		}

		if ( vt.m_fZ > m_fZ )
		{
			m_fZ = vt.m_fZ;
		}
	}

	C3DVector C3DVector::perpendicular() const
	{
		C3DVector vPerp = this->crossProduct( C3DVector::UNIT_X );

		// Check length
		if ( Math::isZero(vPerp.squaredLength()))
		{
			// This vector is the Y axis multiplied by a scalar, so we have to use another axis.
			vPerp = this->crossProduct( C3DVector::UNIT_Y );
		}

		vPerp.normalise();

		return vPerp;
	}

	C3DVector C3DVector::reflect(const C3DVector& vtNormal) const
	{
		return C3DVector( *this - ( vtNormal * (this->dotProduct(vtNormal) * 2) ) );
	}

	C3DVector C3DVector::primaryAxis() const
	{
		FLOAT fAbsX = Math::abs(m_fX);
		FLOAT fAbsY = Math::abs(m_fY);
		FLOAT fAbsZ = Math::abs(m_fZ);

		if (fAbsX > fAbsY)
		{
			if (fAbsX > fAbsZ)
			{
				return m_fX > 0 ? C3DVector::UNIT_X : C3DVector::NEGATIVE_UNIT_X;
			}
			else
			{
				return m_fZ > 0 ? C3DVector::UNIT_Z : C3DVector::NEGATIVE_UNIT_Z;
			}
		}
		else
		{
			if (fAbsY > fAbsZ)
			{
				return m_fY > 0 ? C3DVector::UNIT_Y : C3DVector::NEGATIVE_UNIT_Y;
			}
			else
			{
				return m_fZ > 0 ? C3DVector::UNIT_Z : C3DVector::NEGATIVE_UNIT_Z;
			}
		}
	}

	FLOAT C3DVector::angleBetween(const C3DVector &vt) const
	{
		return 0;
	}

	C3DQuaternion C3DVector::getRotationTo(const C3DVector &vtDest, const C3DVector vtFallbackAxis) const
	{
		return C3DQuaternion();
	}

	BOOLEAN C3DVector::positionEquals(const C3DVector &vt, FLOAT fTolerance) const
	{
		return FALSE;
	}

	BOOLEAN C3DVector::positionCloses(const C3DVector &vt, FLOAT fTolerance) const
	{
		return FALSE;
	}

	BOOLEAN C3DVector::directionEquals(const C3DVector &vt, FLOAT fTolerance) const
	{
		return FALSE;
	}

	BOOLEAN C3DVector::isNaN() const
	{
		return Math::isNaN(m_fX) || Math::isNaN(m_fY) || Math::isNaN(m_fZ);
	}

	C3DVector& C3DVector::operator =(const C3DVector &src)
	{
		m_fX = src.m_fX;
		m_fY = src.m_fY;
		m_fZ = src.m_fZ;

		return *this;
	}

	BOOLEAN C3DVector::operator ==(const C3DVector &src) const
	{
		return	   Math::isZero(m_fX - src.m_fX) 
				&& Math::isZero(m_fY - src.m_fY) 
				&& Math::isZero(m_fZ - src.m_fZ);
	}

	BOOLEAN C3DVector::operator !=(const C3DVector &src) const
	{
		return	!operator==(src);
	}

	C3DVector C3DVector::operator +(const C3DVector &src) const
	{
		return C3DVector(m_fX + src.m_fX, m_fY + src.m_fY, m_fZ + src.m_fZ);
	}

	C3DVector C3DVector::operator -(const C3DVector &src) const
	{
		return C3DVector(m_fX - src.m_fX, m_fY - src.m_fY, m_fZ - src.m_fZ);
	}

	C3DVector C3DVector::operator *(FLOAT fScalar) const
	{
		return C3DVector(m_fX * fScalar, m_fY * fScalar, m_fZ * fScalar);
	}

	C3DVector C3DVector::operator *(const C3DVector &src) const
	{
		return C3DVector(m_fX * src.m_fX, m_fY * src.m_fY, m_fZ * src.m_fZ);
	}

	C3DVector C3DVector::operator /(FLOAT fScalar) const
	{
		FLOAT fTemp = 1.0f / fScalar;
		return C3DVector(m_fX * fTemp, m_fY * fTemp, m_fZ * fTemp);
	}

	C3DVector C3DVector::operator /(const C3DVector &src) const
	{
		return C3DVector(m_fX / src.m_fX, m_fY / src.m_fY, m_fZ / src.m_fZ);
	}

	C3DVector C3DVector::operator +() const
	{
		return C3DVector(m_fX, m_fY, m_fZ);
	}

	C3DVector C3DVector::operator -() const
	{
		return C3DVector(-m_fX, -m_fY, -m_fZ);
	}

	C3DVector& C3DVector::operator +=(const C3DVector &src)
	{
		m_fX += src.m_fX;
		m_fY += src.m_fY;
		m_fZ += src.m_fZ;

		return *this;
	}

	C3DVector& C3DVector::operator -=(const C3DVector &src)
	{
		m_fX -= src.m_fX;
		m_fY -= src.m_fY;
		m_fZ -= src.m_fZ;

		return *this;
	}

	C3DVector& C3DVector::operator *=(FLOAT fScalar)
	{
		m_fX *= fScalar;
		m_fY *= fScalar;
		m_fZ *= fScalar;

		return *this;
	}

	C3DVector& C3DVector::operator *=(const C3DVector &src)
	{
		m_fX *= src.m_fX;
		m_fY *= src.m_fY;
		m_fZ *= src.m_fZ;

		return *this;
	}

	C3DVector& C3DVector::operator /=(FLOAT fScalar)
	{
		FLOAT fTemp = 1.0f / fScalar;

		m_fX *= fTemp;
		m_fY *= fTemp;
		m_fZ *= fTemp;

		return *this;
	}

	C3DVector& C3DVector::operator /=(const C3DVector &src)
	{
		m_fX /= src.m_fX;
		m_fY /= src.m_fY;
		m_fZ /= src.m_fZ;

		return *this;
	}

	BOOLEAN C3DVector::operator <(const C3DVector &src) const
	{
		if (m_fX < src.m_fX && m_fY < src.m_fY && m_fZ < src.m_fZ)
		{
			return TRUE;
		}

		return FALSE;
	}

	BOOLEAN C3DVector::operator >(const C3DVector &src) const
	{
		if (m_fX > src.m_fX && m_fY > src.m_fY && m_fZ > src.m_fZ)
		{
			return TRUE;
		}

		return FALSE;
	}
}