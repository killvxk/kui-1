/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CDiscreteInterpolation : public CBaseInterpolation
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CDiscreteInterpolation();
		virtual ~CDiscreteInterpolation();		

		virtual CBaseAmtData* getInterpolatedValue(LONG lTime);

	protected:// method
		CDiscreteInterpolation(const CDiscreteInterpolation&){}
		CDiscreteInterpolation& operator =(const CDiscreteInterpolation&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}