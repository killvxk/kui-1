#include "OpenHMI.h"

namespace OpenHMI
{
	const	C3DQuaternion	C3DQuaternion::ZERO( 0.0f, 0.0f, 0.0f, 0.0f );
	const	C3DQuaternion	C3DQuaternion::IDENTITY( 0.0f, 0.0f, 0.0f, 1.0f );
	const	FLOAT			C3DQuaternion::EPSILON_VALUE	= 0.001f;

	C3DQuaternion::C3DQuaternion()
		: m_fX(0.0f)
		, m_fY(0.0f)
		, m_fZ(0.0f)
		, m_fW(1.0f)
	{
		;
	}

	C3DQuaternion::C3DQuaternion(const C3DQuaternion &src) 
		: m_fX(src.m_fX)
		, m_fY(src.m_fY)
		, m_fZ(src.m_fZ)
		, m_fW(src.m_fW)
	{
		;
	}

	C3DQuaternion::C3DQuaternion(FLOAT fX, FLOAT fY, FLOAT fZ, FLOAT fW)
		: m_fX(fX)
		, m_fY(fY)
		, m_fZ(fZ)
		, m_fW(fW)
	{
		;
	}

	C3DQuaternion::C3DQuaternion(const C3DVector &vtAxis, FLOAT fAngle)
	{
		fromAxisAngle(vtAxis, fAngle);
	}

	C3DQuaternion::C3DQuaternion(const C3DVector &vtAxisX, const C3DVector &vtAxisY, const C3DVector &vtAxisZ)
	{
		fromAxes(vtAxisX, vtAxisY, vtAxisZ);
	}

	C3DQuaternion::~C3DQuaternion()
	{
		;
	}

	C3DQuaternion C3DQuaternion::slerp(FLOAT fT, const C3DQuaternion &qtnP, const C3DQuaternion &qtnQ, BOOLEAN bShortestPath)
	{
		return C3DQuaternion::ZERO;
	}

	C3DQuaternion C3DQuaternion::slerpExtraSpins(FLOAT fT, const C3DQuaternion &qtnP, const C3DQuaternion &qtnQ, INT iExtraSpins)
	{
		return C3DQuaternion::ZERO;
	}

	void C3DQuaternion::intermediate(const C3DQuaternion &qtn0, const C3DQuaternion &qtn1, const C3DQuaternion &qtn2, C3DQuaternion &qtnA, C3DQuaternion &qtnB)
	{
		;
	}

	C3DQuaternion C3DQuaternion::squad(FLOAT fT, const C3DQuaternion &qtnP, const C3DQuaternion &qtnA, const C3DQuaternion &qtnB, const C3DQuaternion &qtnQ, BOOLEAN bShortestPath)
	{
		return C3DQuaternion::ZERO;
	}

	C3DQuaternion C3DQuaternion::nlerp(FLOAT fT, const C3DQuaternion &qtnP, const C3DQuaternion &qtnQ, BOOLEAN bShortestPath)
	{
		return C3DQuaternion::ZERO;
	}

	void C3DQuaternion::fromAxisAngle(const C3DVector &vtAxis, FLOAT fAngle)
	{
		;
	}

	void C3DQuaternion::toAxisAngle(C3DVector &vtAxis, FLOAT &fAngle) const
	{
		;
	}

	void C3DQuaternion::fromAxes(const C3DVector &vtAxisX, const C3DVector &vtAxisY, const C3DVector &vtAxisZ)
	{
		;
	}

	void C3DQuaternion::toAxes(C3DVector &vtAxisX, C3DVector &vtAxisY, C3DVector &vtAxisZ) const
	{
		;
	}

	C3DVector C3DQuaternion::xAxis() const
	{
		return C3DVector::ZERO;
	}

	C3DVector C3DQuaternion::yAxis() const
	{
		return C3DVector::ZERO;
	}

	C3DVector C3DQuaternion::zAxis() const
	{
		return C3DVector::ZERO;
	}

	FLOAT C3DQuaternion::getRoll(BOOLEAN bReprojectAxis) const
	{
		return 0;
	}

	FLOAT C3DQuaternion::getPitch(BOOLEAN bReprojectAxis) const
	{
		return 0;
	}

	FLOAT C3DQuaternion::getYaw(BOOLEAN bReprojectAxis) const
	{
		return 0;
	}

	FLOAT C3DQuaternion::dotProduct(const C3DQuaternion &qtn) const
	{
		return m_fX * qtn.m_fX + m_fY * qtn.m_fY + m_fZ * qtn.m_fZ + m_fW * qtn.m_fW;
	}

	FLOAT C3DQuaternion::norm() const
	{
		return m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ + m_fW * m_fW;
	}

	FLOAT C3DQuaternion::normalise()
	{
		FLOAT fLen = norm();

		if (fLen > 0.0f)
		{
			FLOAT fFactor = 1.0f / (FLOAT)Math::sqrt(fLen);
			m_fX *= fFactor;
			m_fY *= fFactor;
			m_fZ *= fFactor;
			m_fW *= fFactor;
		}

		return fLen;
	}

	C3DQuaternion C3DQuaternion::normalisedCopy() const
	{
		C3DQuaternion qtn = *this;
		qtn.normalise();
		return qtn;
	}

	C3DQuaternion C3DQuaternion::inverse() const
	{
		return C3DQuaternion::ZERO;
	}

	C3DQuaternion C3DQuaternion::unitInverse() const
	{
		return C3DQuaternion::ZERO;
	}

	C3DQuaternion C3DQuaternion::exp() const
	{
		return C3DQuaternion::ZERO;
	}

	C3DQuaternion C3DQuaternion::log() const
	{
		return C3DQuaternion::ZERO;
	}

	BOOLEAN C3DQuaternion::equals(const C3DQuaternion &qtn, FLOAT fTolerance) const
	{
		return FALSE;
	}

	BOOLEAN C3DQuaternion::isNaN() const
	{
		return Math::isNaN(m_fX) || Math::isNaN(m_fY) || Math::isNaN(m_fZ) || Math::isNaN(m_fW);
	}

	C3DQuaternion& C3DQuaternion::operator =(const C3DQuaternion &src)
	{
		m_fX = src.m_fX;
		m_fY = src.m_fY;
		m_fZ = src.m_fZ;
		m_fW = src.m_fW;

		return *this;
	}

	BOOLEAN C3DQuaternion::operator ==(const C3DQuaternion &src) const
	{
		return	   Math::isZero(m_fX - src.m_fX) 
				&& Math::isZero(m_fY - src.m_fY) 
				&& Math::isZero(m_fZ - src.m_fZ)
				&& Math::isZero(m_fW - src.m_fW);
	}

	BOOLEAN C3DQuaternion::operator !=(const C3DQuaternion &src) const
	{
		return	!operator==(src);
	}

	C3DQuaternion C3DQuaternion::operator +(const C3DQuaternion &src) const
	{
		return C3DQuaternion(m_fX + src.m_fX, m_fY + src.m_fY, m_fZ + src.m_fZ, m_fZ + src.m_fW);
	}

	C3DQuaternion C3DQuaternion::operator -(const C3DQuaternion &src) const
	{
		return C3DQuaternion(m_fX - src.m_fX, m_fY - src.m_fY, m_fZ - src.m_fZ, m_fZ - src.m_fW);
	}

	C3DQuaternion C3DQuaternion::operator *(const C3DQuaternion &src) const
	{
		return C3DQuaternion
			(			
				m_fW * src.m_fX + m_fX * src.m_fW + m_fY * src.m_fZ - m_fZ * src.m_fY,
				m_fW * src.m_fY + m_fY * src.m_fW + m_fZ * src.m_fX - m_fX * src.m_fZ,
				m_fW * src.m_fZ + m_fZ * src.m_fW + m_fX * src.m_fY - m_fY * src.m_fX,
				m_fW * src.m_fW - m_fX * src.m_fX - m_fY * src.m_fY - m_fZ * src.m_fZ
			);
	}

	C3DQuaternion C3DQuaternion::operator *(FLOAT fScalar) const
	{
		return C3DQuaternion(m_fX * fScalar, m_fY * fScalar, m_fZ * fScalar, m_fW * fScalar);
	}

	C3DQuaternion C3DQuaternion::operator +() const
	{
		return C3DQuaternion(m_fX, m_fY, m_fZ, m_fW);
	}

	C3DQuaternion C3DQuaternion::operator -() const
	{
		return C3DQuaternion(-m_fX, -m_fY, -m_fZ, -m_fW);
	}

	C3DVector C3DQuaternion::operator *(const C3DVector &vt) const
	{
		C3DVector uvt, uuvt;
		C3DVector qvec(m_fX, m_fY, m_fZ);
		uvt = qvec.crossProduct(vt);
		uuvt = qvec.crossProduct(uvt);
		uvt *= (2.0f * m_fW);
		uuvt *= 2.0f;

		return vt + uvt + uuvt;
	}
}