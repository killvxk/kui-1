#pragma once

#include "ntfspublic.h"

#include <string>
#include <map>

START_NAMESPACE_NTFS

class UserDiskCopyDirectoryOption
{
public:

	UserDiskCopyDirectoryOption(LPCTSTR lpServerIp, WORD m_nPort);

	~UserDiskCopyDirectoryOption();

public:

	virtual DWORD UserDiskCopyDirectoryOptionCallBack(LPCTSTR lpSrc, LPCTSTR lpDest, DWORD dwWriteBytes);

	virtual DWORD FinishCopyDirListCallBack(LPCTSTR lpSrc, LPCTSTR lpDest);

public:

	void AddDirectoryCopyOpt(LPCTSTR lpDiskDirectory, LPCTSTR lpNtfsDirectory);

	DWORD DoExcute();

	void SetError(DWORD dwError, LPCTSTR szError) 
	{
		m_dwError = dwError;
		lstrcpy(m_strError, szError);
	}

	LPCTSTR GetError() { return m_strError; }

	DWORD GetErrorId() { return m_dwError; }

public:

	std::wstring m_strServerIp;
	WORD m_nPort;
	DWORD m_dwError;
	TCHAR m_strError[1024];
	std::map<std::wstring, std::wstring> m_mapCopyDirList;
};


END_NAMESPACE_NTFS