#pragma once

#include "ntfspublic.h"

struct hive; 
START_NAMESPACE_NTFS

#define MAX_REG_SIZEEX		1 * 1024 * 1024		// 1MB

class hivereg
{
public:

	hivereg(void);

	virtual ~hivereg(void);

public:

	bool OpenReg(LPCTSTR lpszFileName, DWORD dwSizeEx = MAX_REG_SIZEEX);

	void CloseReg();

	bool ExportKey(LPCTSTR lpszKeyPath, LPCTSTR lpszFileName, LPCTSTR lpszPrefix);

	bool OpenKey(LPCTSTR lpszKeyPath);

	bool GetDword(LPCTSTR lpszKeyName, DWORD& val);

	bool GetString(LPCTSTR lpszKeyName, LPWSTR pszValue, DWORD dwSize);

	bool QueryExpandString(LPCTSTR lpszKeyName, LPWSTR pszValue, DWORD dwSize);

	bool QueryMultiString(LPCTSTR lpszKeyName, std::vector<std::wstring>& ms);

	bool QueryBinary(LPCTSTR lpszKeyName, BYTE* out_buf, PDWORD buf_len);

public:

	bool CreateKey(LPCTSTR lpszKeyPath);

	bool ImportKey(LPCTSTR lpszFileName, LPCTSTR lpszPrefix, HANDLE hEvent = NULL,
		LPTSTR lpszError = NULL, DWORD dwLen = 0);

	bool SetDword(LPCTSTR lpszKeyName, DWORD val);

	bool SetString(LPCTSTR lpszKeyName, LPCWSTR pszValue, DWORD dwSize = 0);

	bool SetExpandString(LPCTSTR lpszKeyName, LPCWSTR pszValue, DWORD dwSize);

	bool SetMultiString(LPCTSTR lpszKeyName, std::vector<std::wstring>& ms);

	bool SetBinary(LPCTSTR lpszKeyName, BYTE* in_buf, DWORD in_len);

	bool DeleteKey(LPCTSTR lpszKeyPath);

	bool RDeleteKey(LPCTSTR lpszKeyPath);

	bool DeleteAllValueForKey(LPCTSTR lpszKeyPath);

	bool DeleteValue(LPCTSTR lpszKeyName);

	bool CommitReg();

	void GetCurrentKey(LPTSTR lpszKeyName, DWORD dwLen);

	void EnumKey(std::vector <std::wstring> &tKeyName);

public:

	virtual int FileOpenCallBack();

	virtual int FileSizeCallBack();

	virtual int FileCloseCallBack();

	virtual int FileReadCallBack(__int64 i64Offset, void *pBuffer, int iSize);

	virtual int FileWriteCallBack(__int64 i64Offset, const void *pBuffer, int iSize);

public:

	static int RegFileOpenCallBack(hive *h);

	static int RegFileSizeCallBack(hive *h);

	static int RegFileCloseCallBack(hive *h);

	static int RegFileReadCallBack(hive *h, __int64 i64Offset, void *pBuffer, int iSize);

	static int RegFileWriteCallBack(hive *h, __int64 i64Offset, const void *pBuffer, int iSize);

public:

	struct hive*	m_hivecallback;

private:

	struct hive*	m_hive;
	int				m_current_ofs;
};

END_NAMESPACE_NTFS