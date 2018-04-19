/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"
#include "../../Platform/ScreenTouch/CCursor.h"

namespace OpenHMI
{
	class OPENHMI_API ICursorTracker
	{
	public:// method
		virtual			~ICursorTracker() {}

		virtual	void	addCursor(const CCursor &cursor) = 0;
		virtual	void	addCursorR(const CCursor &cursor) = 0;
		virtual	void	updateCursor(const CCursor &cursor) = 0;
		virtual	void	removeCursor(const CCursor &cursor) = 0;
		virtual	void	removeCursorR(const CCursor &cursor) = 0;

		virtual BOOLEAN	isSupportMultiComponentTouched() = 0;
		virtual BOOLEAN	isDraging() = 0;
	};
}