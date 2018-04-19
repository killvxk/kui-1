#include "OpenHMI.h"

namespace OpenHMI
{
	CEventDispatcher::CEventDispatcher() 
		: m_aryEventListener(FALSE)
	{
		;
	}

	CEventDispatcher::~CEventDispatcher()
	{
		;
	}

	void CEventDispatcher::addEventListener(IEventListener *pListener)
	{
		if (pListener != NULL && !m_aryEventListener.isExist(pListener))
		{
			m_aryEventListener.addItem(pListener);
		}
	}

	void CEventDispatcher::removeEventListener(IEventListener *pListener)
	{
		m_aryEventListener.deleteItem(pListener);
	}

	void CEventDispatcher::dispatchEvent(Object *pTarget, UINT uiEventID, Object *pData)
	{
		m_aryEventListener.resetIterator();
		while (m_aryEventListener.hasMore())
		{
			m_aryEventListener.next()->onDispatchEvent(pTarget, uiEventID, pData);
		}
	}
}