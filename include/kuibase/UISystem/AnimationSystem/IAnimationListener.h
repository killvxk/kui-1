/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IAnimationListener
	{
	public:// method
		virtual			~IAnimationListener() {}

		virtual	void	onAnimationStarted(CBaseAnimation* pSource) = 0;
		virtual	void	onAnimationValueChanged(CBaseAnimation* pSource) = 0;
		virtual	void	onAnimationEnded(CBaseAnimation* pSource) = 0;
	};
}