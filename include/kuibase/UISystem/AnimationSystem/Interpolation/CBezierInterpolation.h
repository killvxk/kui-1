/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBezierInterpolation : public CBaseInterpolation
	{
	public:// const define

	private:// const define
	
	public:// embed class or struct or enum				

	private:// embed class or struct or enum
	
	public:// method	
		CBezierInterpolation(FLOAT fTimePct1, FLOAT fAmtPct1, FLOAT fTimePct2, FLOAT fAmtPct2);
		virtual ~CBezierInterpolation();		
		virtual CBaseAmtData* getInterpolatedValue(LONG lTime);
		
	protected:// method
		CBezierInterpolation(const CBezierInterpolation&){}
		CBezierInterpolation& operator =(const CBezierInterpolation&){return *this;}

	private:// method

	protected:// property

	private:// property
	       Array2<FLOAT> m_aryAmtRat;
	};
}