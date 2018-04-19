/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseMotionAnimation : public CBaseAnimation
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		virtual ~CBaseMotionAnimation();
		
		void setAnimationData(CAmtPoint* pAmtpData);

	protected:// method
		CBaseMotionAnimation();
		
	private:// method

	protected:// property

	private:// property

	};
}