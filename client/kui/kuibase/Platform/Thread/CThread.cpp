#include "OpenHMI.h"

#ifdef OS_WIN
	#include <process.h>
#endif

#ifdef OS_QNX
	
#endif

#ifdef OS_LINUX
	 #include <errno.h>
#endif

namespace OpenHMI
{
	void CThread::prepareStop()
	{
		m_bPrepareStop = TRUE;
		m_waitObject.notify();
	}

	void CThread::stop()
	{
		if (m_bRunning)
		{
			prepareStop();

			m_waitQuit.wait();
		}
	}

#ifdef OS_WIN
	CThread::CThread()
		: m_iPriority(51)
		, m_bRunning(FALSE)
		, m_bPrepareStop(FALSE)		
	{
		;
	}

	CThread::~CThread()
	{
		;
	}

	void CThread::start()
	{
		if (m_bRunning)
		{
			return;
		}

		m_bRunning = TRUE;
		m_bPrepareStop = FALSE;

		UINT uiThreadID = 0;
		HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, threadProc, this, 0, &uiThreadID));
		//HANDLE hThread = reinterpret_cast<HANDLE>(::CreateThread(0, 0, threadProc, (void*)this, 0, &uiThreadID));
		if (hThread != NULL)
		{
			//::SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);

			::CloseHandle(hThread);
			hThread = NULL;
		}
	}

	UINT __stdcall CThread::threadProc(void *pParam)
	{
		CThread *pThread = static_cast<CThread*>(pParam);

		pThread->run();

		pThread->m_bRunning = FALSE;
		pThread->m_waitQuit.notify();

		return 0;
	}
#endif

#ifdef OS_QNX
	CThread::CThread()
		: m_iPriority(51)
		, m_bRunning(FALSE)
		, m_bPrepareStop(FALSE)
	{
		pthread_attr_init(&m_threadAttr);

		m_threadId = NULL;
	}

	CThread::~CThread()
	{
		pthread_attr_destroy(&m_threadAttr);
		m_threadId = NULL;
	}

	void CThread::start()
	{
		if (m_bRunning)
		{
			return;
		}

		m_bRunning = TRUE;
		m_bPrepareStop = FALSE;

		//pthread_attr_setinheritsched(&m_threadAttr, PTHREAD_EXPLICIT_SCHED); 
		//struct sched_param tagParam;
		//tagParam.sched_priority = m_iPriority;   
		//pthread_attr_setschedparam(&m_threadAttr, &tagParam);  
		  
		pthread_create(&m_threadId, &m_threadAttr, threadProc, this);
	}

	void* CThread::threadProc(void *pParam)
	{
		CThread *pThread = static_cast<CThread*>(pParam);
		
		int32_t schedPolicy;
		struct sched_param schedParam;
		if( 0 == pthread_getschedparam( pthread_self(), &schedPolicy, &schedParam ) )
		{
		  schedParam.sched_priority = pThread->m_iPriority;
		  if ( 0 == pthread_setschedparam(pthread_self(), schedPolicy, &schedParam) )
		  {
			  ;
		  }
		}

		pThread->run();
		
		pThread->m_bRunning = FALSE;
		pThread->m_waitQuit.notify();

		return pThread;
	}
#endif

#ifdef OS_LINUX
	CThread::CThread()
		: m_iPriority(51)
		, m_bRunning(FALSE)
		, m_bPrepareStop(FALSE)
	{
		pthread_attr_init(&m_threadAttr);

		m_threadId = NULL;
	}

	CThread::~CThread()
	{
		pthread_attr_destroy(&m_threadAttr);
		m_threadId = NULL;
	}

	void CThread::start()
	{
		if (m_bRunning)
		{
			return;
		}

		m_bRunning = TRUE;
		m_bPrepareStop = FALSE;

		//pthread_attr_setinheritsched(&m_threadAttr, PTHREAD_EXPLICIT_SCHED); 
		//struct sched_param tagParam;
		//tagParam.sched_priority = m_iPriority;   
		//pthread_attr_setschedparam(&m_threadAttr, &tagParam);  

		//String strLog = String::format(L"----- m_iPriority=%d ",
		//	m_iPriority);  
		//CLog::info(L"HMI Thread", (const CHAR*)strLog);

		pthread_create(&m_threadId, &m_threadAttr, threadProc, this);
	}

	void* CThread::threadProc(void *pParam)
	{
		CThread *pThread = static_cast<CThread*>(pParam);
		
		int32_t schedPolicy;
		struct sched_param schedParam;
		if( 0 == pthread_getschedparam( pthread_self(), &schedPolicy, &schedParam ) )
		{
		  schedParam.sched_priority = pThread->m_iPriority;
		  if ( 0 == pthread_setschedparam(pthread_self(), schedPolicy, &schedParam) )
		  {
			  ;
		  }
		}

		pThread->run();
		
		pThread->m_bRunning = FALSE;
		pThread->m_waitQuit.notify();

		return pThread;
	}
#endif

}