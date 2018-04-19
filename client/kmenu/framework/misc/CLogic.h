#pragma once

#include "OpenHMI.h"

namespace OpenHMI
{
	class CLogic
	{
	public:
		static	void	registerAll();
		static	void	unRegisterAll();
	};
}