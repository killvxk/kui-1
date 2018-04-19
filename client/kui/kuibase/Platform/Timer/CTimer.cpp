#include "OpenHMI.h"

namespace OpenHMI
{
	CTimer::CTimer()
		: m_pListener(NULL)
		, m_uiDelay(0)
		, m_uiRepeatCount(0)
		, m_uiCurrentCount(0)
		, m_bRunning(FALSE)
		,m_uiInterval(0)
	{
		;
	}

	CTimer::CTimer(UINT uiDelay, UINT uiRepeatCount)
		: m_pListener(NULL)
		, m_uiDelay(uiDelay)
		, m_uiRepeatCount(uiRepeatCount)
		, m_uiCurrentCount(0)
		, m_bRunning(FALSE)
		,m_uiInterval(0)
	{
		;
	}

	CTimer::~CTimer()
	{
		if( m_bRunning )
		{
			stop();
		}
	}

	void CTimer::setDelay(UINT uiValue)
	{
		m_uiDelay = uiValue;
	}

	void CTimer::setRepeatCount(UINT uiValue)
	{
		m_uiRepeatCount = uiValue;
		m_uiCurrentCount = 0;
	}

	void CTimer::reset()
	{
		m_uiCurrentCount = 0;
		m_uiInterval = 0;
	}

	void CTimer::start()
	{
		if (m_bRunning == FALSE)
		{
			m_bRunning = TRUE;
			m_uiInterval = 0;
			CTimerManager::getInstance()->addTimer(this);
		}
		else
		{
			m_uiInterval = 0;
		}
	}

	void CTimer::stop()
	{
		m_bRunning = FALSE;

		m_uiInterval = 0;

		CTimerManager::getInstance()->removeTimer(this);

		CMain::getInstance()->deleteMessageByID(reinterpret_cast<UINT>(this));
	}

	void CTimer::increaseCurrentCount()
	{
		if (!m_bRunning)
		{
			return;
		}

		m_uiCurrentCount++;

		if (m_uiRepeatCount == 0)
		{
			CMsgTimer *pMessage = new CMsgTimer(this, m_pListener, TS_TIMER, m_uiCurrentCount);
			CMain::getInstance()->pushMessage(pMessage);
		}
		else
		{
			if (m_uiCurrentCount < m_uiRepeatCount)
			{
				CMsgTimer *pMessage = new CMsgTimer(this, m_pListener, TS_TIMER, m_uiCurrentCount);
				CMain::getInstance()->pushMessage(pMessage);
			}
			else if (m_uiCurrentCount == m_uiRepeatCount)
			{
				CMsgTimer *pMessage = new CMsgTimer(this, m_pListener, TS_TIMER_COMPLETE, m_uiCurrentCount);
				CMain::getInstance()->pushMessage(pMessage);

				m_bRunning = FALSE;
			}
			else
			{
				m_bRunning = FALSE;
			}
		}
	}

	BOOLEAN CTimer::isTimeOut(UINT uiTickets)
	{
		if (!m_bRunning)
		{
			return FALSE;
		}

		m_uiInterval += uiTickets;

		if (m_uiInterval >= m_uiDelay)
		{
			m_uiInterval = 0;

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}