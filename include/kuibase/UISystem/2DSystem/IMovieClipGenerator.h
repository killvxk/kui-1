/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IMovieClipGenerator
	{
	public:// method
		virtual					~IMovieClipGenerator() {}

		virtual	CMovieClip*	createMovieClip(UINT uiImageID) = 0;
	};
}