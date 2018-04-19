#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseMotionAnimation::CBaseMotionAnimation()
	{
		;
	}

	CBaseMotionAnimation::~CBaseMotionAnimation()
	{
		;
	}

	void CBaseMotionAnimation::setAnimationData(CAmtPoint* pAmtpData)
	{
		CBaseAnimation::setAnimationData(pAmtpData);
	}
}