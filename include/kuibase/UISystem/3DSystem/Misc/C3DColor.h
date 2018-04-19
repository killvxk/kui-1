/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API C3DColor : public Object
    {
	public:// const define
		static	const C3DColor	WHITE;
		static	const C3DColor	BLACK;
		static	const C3DColor	RED;
		static	const C3DColor	GREEN;
		static	const C3DColor	BLUE;

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		C3DColor();
		C3DColor(const C3DColor &src);
		C3DColor(FLOAT fA, FLOAT fR, FLOAT fG, FLOAT fB);
		C3DColor(UINT uiValue);
		virtual ~C3DColor();

		void	setA(FLOAT fValue)	{ m_fA = fValue > 1 ? 1 : (fValue < 0 ? 0: fValue); }
		FLOAT	getA() const		{ return m_fA; }
		void	setR(FLOAT fValue)	{ m_fR = fValue > 1 ? 1 : (fValue < 0 ? 0: fValue); }
		FLOAT	getR() const		{ return m_fR; }
		void	setG(FLOAT fValue)	{ m_fG = fValue > 1 ? 1 : (fValue < 0 ? 0: fValue); }
		FLOAT	getG() const		{ return m_fG; }
		void	setB(FLOAT fValue)	{ m_fB = fValue > 1 ? 1 : (fValue < 0 ? 0: fValue); }
		FLOAT	getB() const		{ return m_fB; }

		void	setAlpha(BYTE btValue)	{ m_fA = (FLOAT)btValue / 255.0f; }
		BYTE	getAlpha()	const		{ return (BYTE)(m_fA * 255); }
		void	setRed(BYTE btValue)	{ m_fR = (FLOAT)btValue / 255.0f; }
		BYTE	getRed()	const		{ return (BYTE)(m_fR * 255); }
		void	setGreen(BYTE btValue)	{ m_fG = (FLOAT)btValue / 255.0f; }
		BYTE	getGreen()	const		{ return (BYTE)(m_fG * 255); }
		void	setBlue(BYTE btValue)	{ m_fB = (FLOAT)btValue / 255.0f; }
		BYTE	getBlue()	const		{ return (BYTE)(m_fB * 255); }

		void	setValue(UINT uiValue);
		UINT	getValue() const;

		C3DColor&	operator =(const C3DColor &src);
		BOOLEAN		operator ==(const C3DColor &src) const;
		BOOLEAN		operator !=(const C3DColor &src) const;

	protected:// method

	private:// method

	protected:// property

	private:// property
		FLOAT	m_fA;
		FLOAT	m_fR;
		FLOAT	m_fG;
		FLOAT	m_fB;
	};
}