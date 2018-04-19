#include "OpenHMI.h"

namespace OpenHMI
{

#ifdef OS_WIN
	CWaitObject::CWaitObject()
	{
		m_handle = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	CWaitObject::~CWaitObject()
	{
		if (m_handle != NULL)
		{
			CloseHandle(m_handle);
		}
	}

	void CWaitObject::wait(UINT uiInterval /*= 0*/)
	{
		if (uiInterval == 0)
		{
			WaitForSingleObject(m_handle, INFINITE);
		}
		else
		{
			WaitForSingleObject(m_handle, uiInterval);
		}

		ResetEvent(m_handle);
	}

	void CWaitObject::notify()
	{
		SetEvent(m_handle);
	}
#endif

#ifdef OS_QNX
	CWaitObject::CWaitObject()
	{
		m_mutex = PTHREAD_MUTEX_INITIALIZER;
		m_cond = PTHREAD_COND_INITIALIZER;
		m_bSignal = FALSE;
	}

	CWaitObject::~CWaitObject()
	{
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_cond);
	}

	void CWaitObject::wait(UINT uiInterval /*= 0*/)
	{
		if (uiInterval == 0)
		{
			pthread_mutex_lock(&m_mutex);

			while (!m_bSignal)
			{				
				pthread_cond_wait(&m_cond, &m_mutex);
			}
			m_bSignal = FALSE;

			pthread_mutex_unlock(&m_mutex);
		}
		else
		{
			struct timeval now;
			struct timespec outtime;

			//clock_gettime(CLOCK_MONOTONIC, &outtime);

			gettimeofday(&now, NULL);
			outtime.tv_sec = now.tv_sec + (uiInterval / 1000);
			outtime.tv_nsec = (now.tv_usec + ((uiInterval % 1000) * 1000)) * 1000;

			if (outtime.tv_nsec >= 1000000000)
			{
				outtime.tv_sec++;
				outtime.tv_nsec -= 1000000000;
			}
			
			pthread_mutex_lock(&m_mutex);

			if (!m_bSignal)
			{
				pthread_cond_timedwait(&m_cond, &m_mutex, &outtime);
			}
			m_bSignal = FALSE;

			pthread_mutex_unlock(&m_mutex);
		}
	}

	void CWaitObject::notify()
	{
		pthread_mutex_lock(&m_mutex);

		m_bSignal = TRUE;
		pthread_cond_signal(&m_cond);

		pthread_mutex_unlock(&m_mutex);	
	}
#endif

#ifdef OS_LINUX
	CWaitObject::CWaitObject()
	{
		m_mutex = PTHREAD_MUTEX_INITIALIZER;
		m_cond = PTHREAD_COND_INITIALIZER;
		m_bSignal = FALSE;
	}

	CWaitObject::~CWaitObject()
	{
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_cond);
	}

	void CWaitObject::wait(UINT uiInterval /*= 0*/)
	{
		if (uiInterval == 0)
		{
			pthread_mutex_lock(&m_mutex);

			while (!m_bSignal)
			{				
				pthread_cond_wait(&m_cond, &m_mutex);
			}
			m_bSignal = FALSE;

			pthread_mutex_unlock(&m_mutex);
		}
		else
		{
			struct timeval now;
			struct timespec outtime;

			//clock_gettime(CLOCK_MONOTONIC, &outtime);

			gettimeofday(&now, NULL);
			outtime.tv_sec = now.tv_sec + (uiInterval / 1000);
			outtime.tv_nsec = (now.tv_usec + ((uiInterval % 1000) * 1000)) * 1000;

			if (outtime.tv_nsec >= 1000000000)
			{
				outtime.tv_sec++;
				outtime.tv_nsec -= 1000000000;
			}
			
			pthread_mutex_lock(&m_mutex);

			if (!m_bSignal)
			{
				pthread_cond_timedwait(&m_cond, &m_mutex, &outtime);
			}
			m_bSignal = FALSE;

			pthread_mutex_unlock(&m_mutex);
		}
	}

	void CWaitObject::notify()
	{
		pthread_mutex_lock(&m_mutex);

		m_bSignal = TRUE;
		pthread_cond_signal(&m_cond);

		pthread_mutex_unlock(&m_mutex);	
	}
#endif
	
}