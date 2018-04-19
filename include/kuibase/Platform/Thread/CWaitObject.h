/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CWaitObject	
	*			It is used to synchronize two threads.
	*/
	class OPENHMI_API CWaitObject : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CWaitObject();
		virtual ~CWaitObject();

		void wait(UINT uiInterval = 0);
		void notify();

	protected:// method
		CWaitObject(const CWaitObject&){}
		CWaitObject& operator =(const CWaitObject&){return *this;}

	private:// method

	protected:// property

	private:// property
		
#ifdef OS_WIN
		HANDLE			m_handle;
#endif

#ifdef OS_QNX
		pthread_mutex_t	m_mutex;
		pthread_cond_t	m_cond;
		BOOLEAN			m_bSignal;
#endif

#ifdef OS_LINUX
		pthread_mutex_t	m_mutex;
		pthread_cond_t	m_cond;
		BOOLEAN			m_bSignal;
#endif
	};
}
