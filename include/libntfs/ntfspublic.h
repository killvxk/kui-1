#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#include <string>
#include <vector>

#define SECTOR_SIZE		512

#ifndef START_NAMESPACE_NTFS
#define START_NAMESPACE_NTFS		namespace libntfs {
#endif
#ifndef END_NAMESPACE_NTFS
#define END_NAMESPACE_NTFS			}
#endif

START_NAMESPACE_NTFS

class NtfsLock
{
private:
	CRITICAL_SECTION m_lock;
public:
	NtfsLock()			{ InitializeCriticalSection(&m_lock);	}
	~NtfsLock()		{ DeleteCriticalSection(&m_lock);		}
	void Lock()		{ EnterCriticalSection(&m_lock);		}
	void UnLock()	{ LeaveCriticalSection(&m_lock);		}
};

template <typename T>
class NtfsObjectLock
{
private:
	T& _lock;
public:
	NtfsObjectLock(T& lock): _lock(lock) { _lock.Lock(); }
	~NtfsObjectLock()	{ _lock.UnLock(); }
};

#define NTFS_OBJECT_LOCK(lock) NtfsObjectLock<NtfsLock> __xlock__(lock)

#ifdef UNICODE
typedef std::wstring ntfs_tstring;
#else
typedef std::stirng ntfs_tstring;
#endif

size_t Ntfs_Split(const ntfs_tstring& str, std::vector<ntfs_tstring>& subs, TCHAR chr);


END_NAMESPACE_NTFS