#include "OpenHMI.h"

namespace OpenHMI
{
	CMsgTimer::CMsgTimer(	
				CTimer					*pTimer,
				ITimerListener			*pListener,
				CTimer::ETimerStatus	eStatus,
				UINT					uiCurrentCount)
		: m_pTimer(pTimer)
		, m_pListener(pListener)
		, m_eStatus(eStatus)
		, m_uiCurrentCount(uiCurrentCount)
	{
		m_eType = CBaseMessage::MT_TIMER;
		m_uiID = reinterpret_cast<UINT>(m_pTimer);
	}

	CMsgTimer::~CMsgTimer()
	{
		;
	}

	void CMsgTimer::handleMessage()
	{
		if (m_pListener == NULL)
		{
			return;
		}

		switch (m_eStatus)
		{
		case CTimer::TS_TIMER:
			m_pListener->onTimer(m_pTimer, m_uiCurrentCount);
			break;
		
		case CTimer::TS_TIMER_COMPLETE:
			m_pListener->onTimerComplete(m_pTimer);
			break;
		}
	}
}