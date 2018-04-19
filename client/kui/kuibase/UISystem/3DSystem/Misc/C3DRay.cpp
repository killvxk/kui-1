#include "OpenHMI.h"

namespace OpenHMI
{
	C3DRay::C3DRay()
		: m_vtOrigin(C3DVector::ZERO)
		, m_vtDirection(C3DVector::UNIT_Z)
	{
		;
	}

	C3DRay::C3DRay(const C3DRay &src)
		: m_vtOrigin(src.m_vtOrigin)
		, m_vtDirection(src.m_vtDirection)
	{
		;
	}

	C3DRay::C3DRay(const C3DVector &vtOrigin, const C3DVector &vtDirection)
		: m_vtOrigin(vtOrigin)
		, m_vtDirection(vtDirection)
	{
		m_vtDirection.normalise();
	}

	C3DRay::~C3DRay()
	{
		;
	}

	C3DVector C3DRay::getPoint(FLOAT fDistance) const
	{
		return C3DVector(m_vtOrigin + (m_vtDirection * fDistance));
	}

	C3DRay& C3DRay::operator =(const C3DRay &src)
	{
		m_vtOrigin = src.m_vtOrigin;
		m_vtDirection = src.m_vtDirection;

		return *this;
	}

	BOOLEAN C3DRay::operator ==(const C3DRay &src) const
	{
		return m_vtOrigin == src.m_vtOrigin && m_vtDirection == src.m_vtDirection;
	}

	BOOLEAN C3DRay::operator !=(const C3DRay &src) const
	{
		return	!operator==(src);
	}

	C3DVector C3DRay::operator *(FLOAT fDistance) const
	{
		return getPoint(fDistance);
	}
}