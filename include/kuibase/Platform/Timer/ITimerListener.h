/**
* @attention Copyright
* Copyright 2013-2014 - All Rights Reserved 
* HARMAN Shanghai - HMI Team of Software
* Email: Author Zeki.Yan
*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	ITimerListener	
	*			It is the timer interface for listening the timer events. Your class should implement these methods.
	*/
	class OPENHMI_API ITimerListener
	{
	public:// method
		virtual			~ITimerListener() {}

		virtual	void	onTimer(const CTimer *pTimer, UINT uiCurrentCount) = 0;
		virtual	void	onTimerComplete(const CTimer *pTimer) = 0;
	};
}