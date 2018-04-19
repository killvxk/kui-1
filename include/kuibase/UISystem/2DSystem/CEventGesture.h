/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CEventGesture	
	*			Define those events directly dispatched from CComponet.
	*/
	class OPENHMI_API CEventGesture
	{
	public:
		static const UINT PRESS					= 100;					
		static const UINT PRESS_RELEASE			= 101;
		static const UINT LONG_PRESS			= 102;
		static const UINT CONTINUE_PRESS		= 103;
		static const UINT LONG_PRESS_RELEASE	= 104;
		static const UINT MOVE					= 105;
		static const UINT MOVE_RELEASE			= 106;
		static const UINT DRAG					= 107;
		static const UINT DRAG_RELEASE			= 108;
		static const UINT DOUBLE_PRESS			= 109;
		static const UINT DOUBLE_PRESS_RELEASE	= 110;
	};
}