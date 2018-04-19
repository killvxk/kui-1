/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IEventListener
	{
	public:// method
		virtual			~IEventListener() {}

		virtual	void	onDispatchEvent(Object *pTarget, UINT uiEventID, Object *pData = NULL) = 0;
	};

	class OPENHMI_API IEventListenerChain
	{
	public:// method
		virtual			~IEventListenerChain() {}

		virtual BOOLEAN	contains(Object *pTarget) = 0;
	};
}