#include "OpenHMI.h"

namespace OpenHMI
{
	C3DBox::C3DBox()
		: m_vtPoint1(C3DVector::ZERO)
		, m_vtPoint2(C3DVector::ZERO)
	{
		;
	}

	C3DBox::C3DBox(const C3DBox &src)
		: m_vtPoint1(src.m_vtPoint1)
		, m_vtPoint2(src.m_vtPoint2)
	{
		;
	}

	C3DBox::C3DBox(const C3DVector &vtPoint1, const C3DVector &vtPoint2)
		: m_vtPoint1(vtPoint1)
		, m_vtPoint2(vtPoint2)
	{
		;
	}

	C3DBox::~C3DBox()
	{
		;
	}

	C3DBox& C3DBox::operator =(const C3DBox &src)
	{
		m_vtPoint1 = src.m_vtPoint1;
		m_vtPoint2 = src.m_vtPoint2;

		return *this;
	}

	BOOLEAN C3DBox::operator ==(const C3DBox &src) const
	{
		return m_vtPoint1 == src.m_vtPoint1 && m_vtPoint2 == src.m_vtPoint2;
	}

	BOOLEAN C3DBox::operator !=(const C3DBox &src) const
	{
		return	!operator==(src);
	}
}