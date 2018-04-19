/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CLinearInterpolation : public CBaseInterpolation
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		CLinearInterpolation();
		virtual ~CLinearInterpolation();
		
		virtual CBaseAmtData* getInterpolatedValue(LONG lTime);

	protected:// method
		CLinearInterpolation(const CLinearInterpolation&){}
		CLinearInterpolation& operator =(const CLinearInterpolation&){return *this;}		

	private:// method

	protected:// property

	private:// property

	};
}