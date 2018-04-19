/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CThread	
	*			It is the thread class. You can inherit from it in order to make your class has thread function.
	*/
	class OPENHMI_API CThread
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		virtual ~CThread();

		void			setPriority(INT iPriority) { m_iPriority  = iPriority; };
		void			start();
		virtual void	stop();
		
	protected:// method
		CThread();
		
		void			prepareStop();

		virtual	void	run() = 0;		

	private:// method

#ifdef OS_WIN
		static UINT __stdcall threadProc(void *pParam);
		
#endif

#ifdef OS_QNX
		static void* threadProc(void *pParam);
		
#endif

#ifdef OS_LINUX
		static void* threadProc(void *pParam);
		
#endif

	protected:// property
		CSyncObject		m_syncObject;
		CWaitObject		m_waitObject;
		CWaitObject		m_waitQuit;

		INT				m_iPriority;
		BOOLEAN			m_bRunning;
		BOOLEAN			m_bPrepareStop;		

	private:// property		

#ifdef OS_WIN

#endif

#ifdef OS_QNX
		pthread_attr_t	m_threadAttr; 
		pthread_t		m_threadId;
#endif

#ifdef OS_LINUX
		pthread_attr_t	m_threadAttr; 
		pthread_t		m_threadId;
#endif
	};
}