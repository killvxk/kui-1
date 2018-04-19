/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CSyncObject
	*			It is used to protect some data, make it can be accessed by one thread as one time.
	*/
	class OPENHMI_API CSyncObject : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CSyncObject();
		virtual ~CSyncObject();

		void lock();
		void unlock();

	protected:// method
		CSyncObject(const CSyncObject&){}
		CSyncObject& operator =(const CSyncObject&){return *this;}

	private:// method

	protected:// property

	private:// property

#ifdef OS_WIN
		CRITICAL_SECTION	m_critical_section;
#endif

#ifdef OS_QNX
		pthread_mutex_t		m_mutex; 
#endif

#ifdef OS_LINUX
		pthread_mutex_t		m_mutex; 
#endif
	};
}