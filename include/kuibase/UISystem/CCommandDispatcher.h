/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CCommandDispatcher
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		virtual ~CCommandDispatcher();

		void	addCommandListener(ICommandListener *pListener);
		void	removeCommandListener(ICommandListener *pListener);

	protected:// method
		CCommandDispatcher();		

		void	dispatchCommand(const String &strCommand, Object *pData = NULL);

	private:// method

	protected:// property

	private:// property
		Array<ICommandListener>	m_aryCommandListener;
	};
}