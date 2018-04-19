/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IScreenTouchListener
	{
	public:// method
		virtual			~IScreenTouchListener() {}

		virtual	void	onCursorMoved(const CCursor &cursor) = 0;
		virtual	void	onCursorPressDown(const CCursor &cursor) = 0;
		virtual	void	onCursorPressDrag(const CCursor &cursor) = 0;
		virtual	void	onCursorPressUp(const CCursor &cursor) = 0;
		virtual	void	onCursorPressDownR(const CCursor &cursor) = 0;
		virtual	void	onCursorPressUpR(const CCursor &cursor) = 0;
		virtual	void	onWheel(const CCursor &cursor) = 0;
		virtual void	onLostFocus() = 0;
	};
}