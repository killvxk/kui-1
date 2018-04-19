/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseGestureRecognizer
		: public Object
		, public IGestureRecognizer
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		virtual ~CBaseGestureRecognizer();

		/**
		* @brief	Dispatch a gesture recognize result event to the main thread.
		*
		* @param	uiGestureID	A ID used to specify the gesture
		* @param	pData	The pointer of the data package of the gesture.
		*/
		void	dispatchGestureResult(UINT uiGestureID, Object *pData);

		virtual	void	addCursor(UINT uiGroupID, const CCursor &cursor)	{}
		virtual	void	updateCursor(UINT uiGroupID, const CCursor &cursor)	{}
		virtual	void	removeCursor(UINT uiGroupID, const CCursor &cursor)	{}

	protected:// method
		CBaseGestureRecognizer();		

	private:// method

	protected:// property

	private:// property
		
	};
}