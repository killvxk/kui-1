/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API ICommandListener
	{
	public:// method
		virtual			~ICommandListener() {}

		virtual	BOOLEAN	onDispatchCommand(const String &strCommand, Object *pData = NULL) = 0;
	};
}