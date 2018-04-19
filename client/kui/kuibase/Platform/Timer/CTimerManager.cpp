#include "OpenHMI.h"

namespace OpenHMI
{
	CTimerManager	*CTimerManager::s_pInstance	= NULL;

	CTimerManager* CTimerManager::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CTimerManager();
		}

		return s_pInstance;
	}

	void CTimerManager::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CTimerManager::CTimerManager()
		: m_aryTimer(FALSE)
	{
		m_uiInterval = 10;
	}

	CTimerManager::~CTimerManager()
	{
		;
	}

	void CTimerManager::addTimer(CTimer *pTimer)
	{
		if (pTimer == NULL)
		{
			return;
		}

		{
			CAutoSync autoSync(m_syncObject);

			m_aryTimer.addItem(pTimer);
		}

		m_waitObject.notify();
	}

	void CTimerManager::removeTimer(CTimer *pTimer)
	{
		if (pTimer == NULL)
		{
			return;
		}

		{
			CAutoSync autoSync(m_syncObject);

			m_aryTimer.deleteItem(pTimer);
		}
	}

	void CTimerManager::stop()
	{
		if (m_bRunning)
		{
			prepareStop();
		
			m_waitTimer.notify();
			
			m_waitQuit.wait();
		}
	}

	void CTimerManager::run()
	{
#ifdef OS_LINUX
	
		while (TRUE)
		{
			m_waitObject.wait();

			if (m_bPrepareStop)
			{
				break;
			}
			
			struct timeval temp;  
			temp.tv_sec = m_uiInterval / 1000;  
			temp.tv_usec = (m_uiInterval % 1000) * 1000; 
			//CTime time = CTime::getCurrentTime();
			//CLog::info(String::format(L"CWaitObject::wait-start: %d", m_uiInterval), String::format(L"%d %d:%d:%d", time.getHour(), time.getMinute(), time.getSecond(), time.getMilliseconds()));
			select(0, NULL, NULL, NULL, &temp);  	
			//CTime etime = CTime::getCurrentTime();
			//CLog::info(String::format(L"CWaitObject::wait-end: %d", m_uiInterval), String::format(L"%d %d:%d:%d", etime.getHour(), etime.getMinute(), etime.getSecond(), etime.getMilliseconds()));
			
			if (m_bPrepareStop)
			{
				break;
			}
			
			CAutoSync autoSync(m_syncObject);

			for (UINT i = 0; i < m_aryTimer.size(); i++)
			{
				CTimer * pTimer = m_aryTimer[i];

				if (pTimer->isTimeOut(m_uiInterval))
				{
					pTimer->increaseCurrentCount();
				}
			}

			if (m_aryTimer.size() > 0)
			{
				m_waitObject.notify();
			}
		}
#else
		while (TRUE)
		{
			m_waitObject.wait();

			if (m_bPrepareStop)
			{
				break;
			}

			m_waitTimer.wait(m_uiInterval);
			
			if (m_bPrepareStop)
			{
				break;
			}
			
			CAutoSync autoSync(m_syncObject);

			for (UINT i = 0; i < m_aryTimer.size(); i++)
			{
				CTimer * pTimer = m_aryTimer[i];

				if (pTimer->isTimeOut(m_uiInterval))
				{
					pTimer->increaseCurrentCount();
				}
			}

			if (m_aryTimer.size() > 0)
			{
				m_waitObject.notify();
			}
		}

#endif
	}
}