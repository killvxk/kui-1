/*@Author Zeki.Yan*/
#pragma once

#if defined(OS_LINUX) || defined(__linux__)
	#include <stddef.h>
#endif

#if defined(OS_QNX) || defined(__QNX__)
	// _QNX_SOURCE defined in "os_defines.h", and <sys/platform.h> is depend on this Micro.
	#define _QNX_SOURCE 1
	#include <stddef.h>
#endif

#if defined(OS_WIN)
	#include <stddef.h>
#endif


#include <new>

namespace OpenHMI
{
	class OPENHMI_API CMemManager
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		static void* alloc(size_t size);
		static void  free(void* p);

	protected:// method
		CMemManager(){}
		CMemManager(const CMemManager&){}
		virtual ~CMemManager(){}		
		CMemManager& operator =(const CMemManager&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}

inline void*	operator new(size_t size)		{ return OpenHMI::CMemManager::alloc(size);		} 
inline void		operator delete(void* p)		{ OpenHMI::CMemManager::free(p);				}
inline void*	operator new[](size_t size)		{ return OpenHMI::CMemManager::alloc(size);		} 
inline void		operator delete[](void* p)		{ OpenHMI::CMemManager::free(p);				}