/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"
#include "../../Platform/ScreenTouch/CCursor.h"

namespace OpenHMI
{
	class OPENHMI_API IGestureRecognizer
	{
	public:// method
		virtual			~IGestureRecognizer() {}

		virtual	void	addCursor(UINT uiGroupID, const CCursor &cursor) = 0;
		virtual	void	updateCursor(UINT uiGroupID, const CCursor &cursor) = 0;
		virtual	void	removeCursor(UINT uiGroupID, const CCursor &cursor) = 0;
	};
}