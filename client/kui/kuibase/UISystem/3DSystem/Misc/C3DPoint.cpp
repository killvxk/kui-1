#include "OpenHMI.h"

namespace OpenHMI
{
	C3DPoint3::C3DPoint3()
		: m_fX(0.0F)
		, m_fY(0.0F)
		, m_fZ(0.0F)
	{
		;
	}

	C3DPoint3::C3DPoint3(const C3DPoint3 &src) 
		: m_fX(src.m_fX)
		, m_fY(src.m_fY)
		, m_fZ(src.m_fZ)
	{
		;
	}

	C3DPoint3::C3DPoint3(FLOAT fX, FLOAT fY, FLOAT fZ)
		: m_fX(fX)
		, m_fY(fY)
		, m_fZ(fZ)
	{
		;
	}

	C3DPoint3::~C3DPoint3()
	{
		;
	}

	void C3DPoint3::offset(FLOAT fX, FLOAT fY, FLOAT fZ)
	{
		m_fX += fX;
		m_fY += fY;
		m_fZ += fZ;
	}

	C3DPoint3& C3DPoint3::operator =(const C3DPoint3 &src)
	{
		m_fX = src.m_fX;
		m_fY = src.m_fY;
		m_fZ = src.m_fZ;

		return *this;
	}

	BOOLEAN C3DPoint3::operator ==(const C3DPoint3 &src) const
	{
		return	   Math::isZero(m_fX - src.m_fX) 
				&& Math::isZero(m_fY - src.m_fY) 
				&& Math::isZero(m_fZ - src.m_fZ);
	}

	BOOLEAN C3DPoint3::operator !=(const C3DPoint3 &src) const
	{
		return	!operator==(src);
	}
}