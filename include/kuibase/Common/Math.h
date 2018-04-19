/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

#ifdef OS_WIN
	#include <cmath>
#endif

#ifdef OS_QNX
	#include <cmath>
#endif

#ifdef OS_LINUX
	#include <cmath>
#endif

namespace OpenHMI
{
	/**
	* @class	Math	
	*			It is a arithmetic class. It defines math formula.
	*/
	class OPENHMI_API Math
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		static	INT		abs(INT iValue)			{ return iValue < 0 ? -iValue : iValue; }
		static	FLOAT	abs(FLOAT fValue)		{ return fValue < 0 ? -fValue : fValue; }
		static	DOUBLE	abs(DOUBLE dValue)		{ return dValue < 0 ? -dValue : dValue; }

		static	INT		getMin(INT iA, INT iB)			{ return iA < iB ? iA : iB; }
		static	UINT	getMin(UINT uiA, UINT uiB)		{ return uiA < uiB ? uiA : uiB; }
		static	FLOAT	getMin(FLOAT fA, FLOAT fB)		{ return fA < fB ? fA : fB; }
		static	DOUBLE	getMin(DOUBLE dA, DOUBLE dB)	{ return dA < dB ? dA : dB; }

		static	INT		getMax(INT iA, INT iB)			{ return iA > iB ? iA : iB; }
		static	UINT	getMax(UINT uiA, UINT uiB)		{ return uiA > uiB ? uiA : uiB; }
		static	FLOAT	getMax(FLOAT fA, FLOAT fB)		{ return fA > fB ? fA : fB; }
		static	DOUBLE	getMax(DOUBLE dA, DOUBLE dB)	{ return dA > dB ? dA : dB; }

		static	void	swapAtMinMax(INT &iA, INT &iB);
		static	void	swapAtMinMax(UINT &uiA, UINT &uiB);
		static	void	swapAtMinMax(FLOAT &fA, FLOAT &fB);
		static	void	swapAtMinMax(DOUBLE &dA, DOUBLE &dB);

		static	INT		pow(INT iValue, UINT uiCount);
		static	FLOAT	pow(FLOAT fValue, UINT uiCount);
		static	DOUBLE	pow(DOUBLE dValue, UINT uiCount);

		static	DOUBLE	sqrt(DOUBLE dValue);

		static	FLOAT	toAngle(FLOAT fRadian)	{ return fRadian * 57.29583f; }
		static	FLOAT	toRadian(FLOAT fAngle)	{ return fAngle * 3.141593f / 180; }

		static	BOOLEAN	isZero(FLOAT fValue)	{ return abs(fValue) < 1.0E-4 ? TRUE : FALSE; }
		static	BOOLEAN	isZero(DOUBLE dValue)	{ return abs(dValue) < 1.0E-8 ? TRUE : FALSE; }

		static	BOOLEAN	isNaN(FLOAT fValue)		{ return fValue != fValue; }
		static	BOOLEAN	isNaN(DOUBLE dValue)	{ return dValue != dValue; }

///////////////////////////////////////////////////////////////////////////////////////////////
		// you must use radian when you use trigonometric functions.
		static	DOUBLE	sin(DOUBLE dValue)		{ return std::sin(dValue); }
		static	DOUBLE	cos(DOUBLE dValue)		{ return std::cos(dValue); }
		static	DOUBLE	tan(DOUBLE dValue)		{ return std::tan(dValue); }

		static	DOUBLE	asin(DOUBLE dValue)		{ return std::asin(dValue); }
		static	DOUBLE	acos(DOUBLE dValue)		{ return std::acos(dValue); }
		static	DOUBLE	atan(DOUBLE dValue)		{ return std::atan(dValue); }
		static	DOUBLE	atan2(DOUBLE dValue1,DOUBLE dValue2)		{ return std::atan2(dValue1,dValue2); }

		static	DOUBLE	sinh(DOUBLE dValue)		{ return std::sinh(dValue); }
		static	DOUBLE	cosh(DOUBLE dValue)		{ return std::cosh(dValue); }
		static	DOUBLE	tanh(DOUBLE dValue)		{ return std::tanh(dValue); }

		static	DOUBLE	log(DOUBLE dValue)		{ return std::log(dValue); }
		static	DOUBLE	log10(DOUBLE dValue)	{ return std::log10(dValue); }

		static	DOUBLE	exp(DOUBLE dValue)		{ return std::exp(dValue); }
	protected:// method
		Math(){}
		Math(const Math&){}
		virtual ~Math(){}		
		Math& operator =(const Math&){return *this;}

	private:// method
		
	protected:// property

	private:// property

	};
}