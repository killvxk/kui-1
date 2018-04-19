/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IRenderable
	{
	public:// method
		virtual			~IRenderable() {}

		virtual	void	buildRenderTree(CRenderGroup &renderGroup) = 0;
	};
}