#include "OpenHMI.h"

namespace OpenHMI
{
#ifdef OS_WIN
	CSyncObject::CSyncObject()
	{
		InitializeCriticalSection(&m_critical_section);
	}

	CSyncObject::~CSyncObject()
	{
		DeleteCriticalSection(&m_critical_section);
	}

	void CSyncObject::lock()
	{
		EnterCriticalSection(&m_critical_section);
	}

	void CSyncObject::unlock()
	{
		LeaveCriticalSection(&m_critical_section);
	}
#endif

#ifdef OS_QNX
	CSyncObject::CSyncObject()
	{
		pthread_mutex_init(&m_mutex, NULL);
	}

	CSyncObject::~CSyncObject()
	{
		pthread_mutex_destroy(&m_mutex);
	}

	void CSyncObject::lock()
	{
		pthread_mutex_lock(&m_mutex);
	}

	void CSyncObject::unlock()
	{
		pthread_mutex_unlock(&m_mutex);
	}
#endif

#ifdef OS_LINUX
	CSyncObject::CSyncObject()
	{
		pthread_mutex_init(&m_mutex, NULL);
	}

	CSyncObject::~CSyncObject()
	{
		pthread_mutex_destroy(&m_mutex);
	}

	void CSyncObject::lock()
	{
		pthread_mutex_lock(&m_mutex);
	}

	void CSyncObject::unlock()
	{
		pthread_mutex_unlock(&m_mutex);
	}
#endif
}