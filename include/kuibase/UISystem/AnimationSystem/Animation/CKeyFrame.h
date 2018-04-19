/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CKeyFrame : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CKeyFrame(LONG lTime, CBaseAmtData* pAmtData);
		virtual 		~CKeyFrame();

		LONG 			getTime();
		CBaseAmtData*	getValue();

	protected:// method
		CKeyFrame(const CKeyFrame&){}
		CKeyFrame& operator =(const CKeyFrame&){return *this;}

	private:// method

	protected:// property

	private:// property
		LONG	m_lTime;
		ObjectPtr<CBaseAmtData>	m_pAmtValue;
	};
}