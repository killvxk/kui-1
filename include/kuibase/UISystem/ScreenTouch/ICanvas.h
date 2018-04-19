/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API ICanvas
	{
	public:// method
		virtual	~ICanvas() {}
		virtual	void move(INT iX, INT iY) = 0;
		virtual	void hoverCheck(INT iX, INT iY) = 0;
		virtual void lostHover() = 0;
		virtual	ICursorTracker*	probe(INT iX, INT iY) = 0;
		virtual	void wheel(CCursor::EWheelType) = 0;
	};
}