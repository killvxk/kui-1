#include "OpenHMI.h"

namespace OpenHMI
{
	CCommandDispatcher::CCommandDispatcher() 
		: m_aryCommandListener(FALSE)
	{
		;
	}

	CCommandDispatcher::~CCommandDispatcher()
	{
		;
	}

	void CCommandDispatcher::addCommandListener(ICommandListener *pListener)
	{
		if (pListener != NULL && !m_aryCommandListener.isExist(pListener))
		{
			m_aryCommandListener.addItem(pListener);
		}
	}

	void CCommandDispatcher::removeCommandListener(ICommandListener *pListener)
	{
		m_aryCommandListener.deleteItem(pListener);
	}

	void CCommandDispatcher::dispatchCommand(const String &strCommand, Object *pData)
	{
		m_aryCommandListener.resetIterator();
		while (m_aryCommandListener.hasMore())
		{
			m_aryCommandListener.next()->onDispatchCommand(strCommand, pData);
		}
	}
}