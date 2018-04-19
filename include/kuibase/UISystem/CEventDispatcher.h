/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventDispatcher
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		virtual ~CEventDispatcher();

		void	addEventListener(IEventListener *pListener);
		void	removeEventListener(IEventListener *pListener);

	protected:// method
		CEventDispatcher();		

		void	dispatchEvent(Object *pTarget, UINT uiEventID, Object *pData = NULL);

	private:// method

	protected:// property

	private:// property
		Array<IEventListener>	m_aryEventListener;
	};
}