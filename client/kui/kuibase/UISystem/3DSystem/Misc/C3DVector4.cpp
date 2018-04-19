#include "OpenHMI.h"

namespace OpenHMI
{
	C3DVector4::C3DVector4()
		: m_fX(0.0f)
		, m_fY(0.0f)
		, m_fZ(0.0f)
		, m_fW(0.0f)
	{
		;
	}

	C3DVector4::C3DVector4(const C3DVector4 &src) 
		: m_fX(src.m_fX)
		, m_fY(src.m_fY)
		, m_fZ(src.m_fZ)
		, m_fW(src.m_fW)
	{
		;
	}

	C3DVector4::C3DVector4(FLOAT fX, FLOAT fY, FLOAT fZ, FLOAT fW)
		: m_fX(fX)
		, m_fY(fY)
		, m_fZ(fZ)
		, m_fW(fW)
	{
		;
	}

	C3DVector4::~C3DVector4()
	{
		;
	}

	FLOAT C3DVector4::length() const
	{
		return (FLOAT)Math::sqrt( m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ + m_fW * m_fW );
	}

	FLOAT C3DVector4::squaredLength() const
	{
		return m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ + m_fW * m_fW;
	}

	BOOLEAN C3DVector4::isZeroLength() const
	{
		return Math::isZero(squaredLength());
	}

	FLOAT C3DVector4::distance(const C3DVector4& vt) const
	{
		return (*this - vt).length();
	}

	FLOAT C3DVector4::squaredDistance(const C3DVector4& vt) const
	{
		return (*this - vt).squaredLength();
	}

	FLOAT C3DVector4::normalise()
	{
		FLOAT fLength = length();

		if ( fLength > 0.0f )
		{
			FLOAT fInvLength = 1.0f / fLength;
			m_fX *= fInvLength;
			m_fY *= fInvLength;
			m_fZ *= fInvLength;
			m_fW *= fInvLength;
		}

		return fLength;
	}

	C3DVector4 C3DVector4::normalisedCopy() const
	{
		C3DVector4 vt = *this;
		vt.normalise();
		return vt;
	}

	FLOAT C3DVector4::dotProduct(const C3DVector4& vt) const
	{
		 return m_fX * vt.m_fX + m_fY * vt.m_fY + m_fZ * vt.m_fZ + m_fW * m_fW;
	}

	FLOAT C3DVector4::absDotProduct(const C3DVector4& vt) const
	{
		return Math::abs(m_fX * vt.m_fX) + Math::abs(m_fY * vt.m_fY) +Math::abs( m_fZ * vt.m_fZ) + Math::abs( m_fW * vt.m_fW);
	}

	BOOLEAN C3DVector4::isNaN() const
	{
		return Math::isNaN(m_fX) || Math::isNaN(m_fY) || Math::isNaN(m_fZ) || Math::isNaN(m_fW);
	}

	C3DVector4& C3DVector4::operator =(const C3DVector4 &src)
	{
		m_fX = src.m_fX;
		m_fY = src.m_fY;
		m_fZ = src.m_fZ;
		m_fW = src.m_fW;

		return *this;
	}

	BOOLEAN C3DVector4::operator ==(const C3DVector4 &src) const
	{
		return	   Math::isZero(m_fX - src.m_fX) 
				&& Math::isZero(m_fY - src.m_fY) 
				&& Math::isZero(m_fZ - src.m_fZ)
				&& Math::isZero(m_fW - src.m_fW);
	}

	BOOLEAN C3DVector4::operator !=(const C3DVector4 &src) const
	{
		return	!operator==(src);
	}

	C3DVector4 C3DVector4::operator +(const C3DVector4 &src) const
	{
		return C3DVector4(m_fX + src.m_fX, m_fY + src.m_fY, m_fZ + src.m_fZ, m_fW + src.m_fW);
	}

	C3DVector4 C3DVector4::operator -(const C3DVector4 &src) const
	{
		return C3DVector4(m_fX - src.m_fX, m_fY - src.m_fY, m_fZ - src.m_fZ, m_fW - src.m_fW);
	}

	C3DVector4 C3DVector4::operator *(FLOAT fScalar) const
	{
		return C3DVector4(m_fX * fScalar, m_fY * fScalar, m_fZ * fScalar, m_fW * fScalar);
	}

	C3DVector4 C3DVector4::operator *(const C3DVector4 &src) const
	{
		return C3DVector4(m_fX * src.m_fX, m_fY * src.m_fY, m_fZ * src.m_fZ, m_fW * src.m_fW);
	}

	C3DVector4 C3DVector4::operator /(FLOAT fScalar) const
	{
		FLOAT fTemp = 1.0f / fScalar;
		return C3DVector4(m_fX * fTemp, m_fY * fTemp, m_fZ * fTemp, m_fW * fTemp);
	}

	C3DVector4 C3DVector4::operator /(const C3DVector4 &src) const
	{
		return C3DVector4(m_fX / src.m_fX, m_fY / src.m_fY, m_fZ / src.m_fZ, m_fW / src.m_fW);
	}

	C3DVector4 C3DVector4::operator +() const
	{
		return C3DVector4(m_fX, m_fY, m_fZ,m_fW);
	}

	C3DVector4 C3DVector4::operator -() const
	{
		return C3DVector4(-m_fX, -m_fY, -m_fZ, -m_fW);
	}

	C3DVector4& C3DVector4::operator +=(const C3DVector4 &src)
	{
		m_fX += src.m_fX;
		m_fY += src.m_fY;
		m_fZ += src.m_fZ;
		m_fW += src.m_fW;

		return *this;
	}

	C3DVector4& C3DVector4::operator -=(const C3DVector4 &src)
	{
		m_fX -= src.m_fX;
		m_fY -= src.m_fY;
		m_fZ -= src.m_fZ;
		m_fW -= src.m_fW;

		return *this;
	}

	C3DVector4& C3DVector4::operator *=(FLOAT fScalar)
	{
		m_fX *= fScalar;
		m_fY *= fScalar;
		m_fZ *= fScalar;
		m_fW *= fScalar;

		return *this;
	}

	C3DVector4& C3DVector4::operator *=(const C3DVector4 &src)
	{
		m_fX *= src.m_fX;
		m_fY *= src.m_fY;
		m_fZ *= src.m_fZ;
		m_fW *= src.m_fW;

		return *this;
	}

	C3DVector4& C3DVector4::operator /=(FLOAT fScalar)
	{
		FLOAT fTemp = 1.0f / fScalar;

		m_fX *= fTemp;
		m_fY *= fTemp;
		m_fZ *= fTemp;
		m_fW *= fTemp;

		return *this;
	}

	C3DVector4& C3DVector4::operator /=(const C3DVector4 &src)
	{
		m_fX /= src.m_fX;
		m_fY /= src.m_fY;
		m_fZ /= src.m_fZ;
		m_fW /= src.m_fW;

		return *this;
	}

	BOOLEAN C3DVector4::operator <(const C3DVector4 &src) const
	{
		if (m_fX < src.m_fX && m_fY < src.m_fY && m_fZ < src.m_fZ && m_fW < src.m_fW)
		{
			return TRUE;
		}

		return FALSE;
	}

	BOOLEAN C3DVector4::operator >(const C3DVector4 &src) const
	{
		if (m_fX > src.m_fX && m_fY > src.m_fY && m_fZ > src.m_fZ && m_fW > src.m_fW)
		{
			return TRUE;
		}

		return FALSE;
	}
}