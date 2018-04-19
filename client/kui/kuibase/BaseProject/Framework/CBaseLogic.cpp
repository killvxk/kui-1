#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseLogic::CBaseLogic()
		: m_bInitialized(FALSE)
	{
		;
	}

	CBaseLogic::~CBaseLogic()
	{
		;
	}

	BEGIN_EVENT_MAP(CBaseLogic)
	END_EVENT_MAP_2

	BOOLEAN CBaseLogic::initialize()
	{
		if (m_bInitialized)
		{
			return FALSE;
		}
		
		return TRUE;
	}

	void CBaseLogic::unInitialize()
	{
		m_bInitialized = FALSE;
	}

	BOOLEAN CBaseLogic::isReady()
	{
		return TRUE;
	}

	void CBaseLogic::dispatchEvent(UINT uiEventID)
	{
		CEventDispatcher::dispatchEvent(this, uiEventID);
	}
}