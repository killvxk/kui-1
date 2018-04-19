#include "OpenHMI.h"

namespace OpenHMI
{
	C3DPlane::C3DPlane()
		: m_vtNormalize(C3DVector::UNIT_Z)
		, m_fDistance(0.0f)
	{
		;
	}

	C3DPlane::C3DPlane(const C3DPlane &src)
		: m_vtNormalize(src.m_vtNormalize)
		, m_fDistance(src.m_fDistance)
	{
		;
	}

	C3DPlane::C3DPlane(const C3DVector &vtNormalize, FLOAT fDistance)
		: m_vtNormalize(vtNormalize)
		, m_fDistance(fDistance)
	{
		m_vtNormalize.normalise();
	}

	C3DPlane::C3DPlane(const C3DVector &vtNormalize, const C3DVector &vtPoint1)
	{
		;
	}

	C3DPlane::C3DPlane(const C3DVector &vtPoint1, const C3DVector &vtPoint2, const C3DVector &vtPoint3)
	{
		;
	}

	C3DPlane::~C3DPlane()
	{
		;
	}

	C3DPlane::EPlaneSide C3DPlane::getSide(const C3DVector &vtPoint) const
	{
		return PS_NO_SIDE;
	}

	C3DPlane::EPlaneSide C3DPlane::getSide(const C3DBox &box) const
	{
		return PS_NO_SIDE;
	}

	C3DPlane::EPlaneSide C3DPlane::getSide(const C3DVector &vtCentre, const C3DVector &vtHalfSize) const
	{
		return PS_NO_SIDE;
	}

	void C3DPlane::redefine(const C3DVector &vtNormalize, const C3DVector &vtPoint1)
	{
		;
	}

	void C3DPlane::redefine(const C3DVector &vtPoint1, const C3DVector &vtPoint2, const C3DVector &vtPoint3)
	{
		;
	}

	FLOAT C3DPlane::getDistance (const C3DVector &vtPoint) const
	{
		return 0.0f;
	}

	C3DVector C3DPlane::projectVector(const C3DVector &vt) const
	{
		return C3DVector::ZERO;
	}

	C3DPlane& C3DPlane::operator =(const C3DPlane &src)
	{
		m_vtNormalize = src.m_vtNormalize;
		m_fDistance = src.m_fDistance;

		return *this;
	}

	BOOLEAN C3DPlane::operator ==(const C3DPlane &src) const
	{
		return m_vtNormalize == src.m_vtNormalize && Math::isZero(m_fDistance - src.m_fDistance);
	}

	BOOLEAN C3DPlane::operator !=(const C3DPlane &src) const
	{
		return	!operator==(src);
	}
}