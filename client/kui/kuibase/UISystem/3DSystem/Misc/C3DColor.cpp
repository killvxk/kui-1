#include "OpenHMI.h"

namespace OpenHMI
{
	const C3DColor	C3DColor::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
	const C3DColor	C3DColor::BLACK(1.0f, 0.0f, 0.0f, 0.0f);
	const C3DColor	C3DColor::RED(1.0f, 1.0f, 0.0f, 0.0f);
	const C3DColor	C3DColor::GREEN(1.0f, 0.0f, 1.0f, 0.0f);
	const C3DColor	C3DColor::BLUE(1.0f, 0.0f, 0.0f, 1.0f);

	C3DColor::C3DColor()
		: m_fA(1.0f)
		, m_fR(1.0f)
		, m_fG(1.0f)
		, m_fB(1.0f)
	{
		;
	}

	C3DColor::C3DColor(const C3DColor &src)
		: m_fA(src.m_fA)
		, m_fR(src.m_fR)
		, m_fG(src.m_fG)
		, m_fB(src.m_fB)
	{
		;
	}

	C3DColor::C3DColor(FLOAT fA, FLOAT fR, FLOAT fG, FLOAT fB)
	{
		m_fA = fA > 1 ? 1 : (fA < 0 ? 0: fA);
		m_fR = fR > 1 ? 1 : (fR < 0 ? 0: fR);
		m_fG = fG > 1 ? 1 : (fG < 0 ? 0: fG);
		m_fB = fB > 1 ? 1 : (fB < 0 ? 0: fB);
	}

	C3DColor::C3DColor(UINT uiValue)
	{
		setValue(uiValue);
	}

	C3DColor::~C3DColor()
	{
		;
	}

	void C3DColor::setValue(UINT uiValue)
	{
		m_fA = ((uiValue >> 24) & 0xFF) / 255.0f; 
		m_fR = ((uiValue >> 16) & 0xFF) / 255.0f; 
		m_fG = ((uiValue >> 8) & 0xFF) / 255.0f; 
		m_fB = ((uiValue) & 0xFF) / 255.0f; 
	}

	UINT C3DColor::getValue() const
	{
		return	  ((UINT)(m_fA * 255) << 24)
				| ((UINT)(m_fR * 255) << 16)
				| ((UINT)(m_fG * 255) << 8)
				| ((UINT)(m_fB * 255));
	}

	C3DColor& C3DColor::operator =(const C3DColor &src)
	{
		m_fA = src.m_fA;
		m_fR = src.m_fR;
		m_fG = src.m_fG;
		m_fB = src.m_fB;

		return *this;
	}

	BOOLEAN C3DColor::operator ==(const C3DColor &src) const
	{
		return	   Math::isZero(m_fA - src.m_fA) 
				&& Math::isZero(m_fR - src.m_fR) 
				&& Math::isZero(m_fG - src.m_fG)
				&& Math::isZero(m_fB - src.m_fB);
	}

	BOOLEAN C3DColor::operator !=(const C3DColor &src) const
	{
		return	!operator==(src);
	}
}