#include "stdafx.h"

START_NAMESPACE_KCORE

class CLogger
{
private:
	CLock		m_lock;
	HANDLE		m_hLogFile;
	SYSTEMTIME	m_SysTime;
	TCHAR		m_logFileModuleName[MAX_PATH];
	TCHAR		m_logFileFullName[MAX_PATH];
public:
	CLogger() : m_hLogFile(INVALID_HANDLE_VALUE)
	{
		ZeroMemory(&m_SysTime, sizeof(m_SysTime));
		ZeroMemory(m_logFileModuleName, sizeof(m_logFileModuleName));
		ZeroMemory(m_logFileFullName, sizeof(m_logFileFullName));
		lstrcpy(m_logFileModuleName, TEXT("bflog"));
		setlocale(LC_ALL ,"chs");
	}
	virtual ~CLogger()
	{
		if (m_hLogFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hLogFile);
			m_hLogFile = INVALID_HANDLE_VALUE;
		}
	}
public:
	void SetLogModule(LPCTSTR szModule)
	{
		OBJECT_LOCK(m_lock);
		if (szModule != NULL && lstrlen(szModule))
		{
			if (m_hLogFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = INVALID_HANDLE_VALUE;
			}

			lstrcpy(m_logFileModuleName, szModule);
		}
	}
	LPCTSTR GetLogFileFullName()
	{
		OBJECT_LOCK(m_lock);

		TCHAR szDir[MAX_PATH] = {0};
		AfxGetAppPath(szDir, MAX_PATH, TEXT("Log\\"));
		CreateDirectory(szDir, NULL);

		SYSTEMTIME st = {0};
		GetLocalTime(&st);
		_stprintf_s(m_logFileFullName, TEXT("%s%s-%04d%02d%02d.log"), 
			szDir, m_logFileModuleName, st.wYear, st.wMonth, st.wDay);
		return m_logFileFullName;
	}
	void WriteLog(LPCTSTR szLog, int nCharsNums = -1)
	{
		OBJECT_LOCK(m_lock);
		if (EnsureOpenFile())
		{
			DWORD dwWriteBytes = 0;
			int nLen = (nCharsNums == -1) ? lstrlen(szLog) : nCharsNums;
			nLen *= sizeof(TCHAR);
			WriteFile(m_hLogFile, szLog, nLen, &dwWriteBytes, NULL);
		}
	}
private:
	bool EnsureOpenFile()
	{
		SYSTEMTIME st = {0};
		GetLocalTime(&st);
		if (st.wYear != m_SysTime.wYear || st.wMonth != m_SysTime.wMonth ||
			st.wDay  != m_SysTime.wDay)
		{
			if (m_hLogFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = INVALID_HANDLE_VALUE;
			}
		}
		if (m_hLogFile == INVALID_HANDLE_VALUE)
		{
			m_hLogFile = CreateFile(GetLogFileFullName(), GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 
				NULL, OPEN_ALWAYS, 0, NULL);
			if (m_hLogFile == INVALID_HANDLE_VALUE)
				return false;

			//write unicode file sig.
#ifdef _UNICODE
			short start = 0;
			DWORD dwBytes = 0;
			if (!ReadFile(m_hLogFile, &start, 2, &dwBytes, NULL) || start != 0xfeff)
			{
				SetFilePointer(m_hLogFile , 0, NULL, FILE_BEGIN);
				start = (short)0xfeff;
				WriteFile(m_hLogFile, &start, sizeof(start), &dwBytes, NULL);
			}
#endif
			SetFilePointer(m_hLogFile, 0, 0, FILE_END);
			m_SysTime = st;
		}
		return true;
	}

};

HANDLE __stdcall AfxCreateLogger(LPCTSTR szModuleName)
{
	CLogger* pLogger = new CLogger;
	pLogger->SetLogModule(szModuleName);

	return reinterpret_cast<HANDLE>(pLogger);
}

void  __cdecl AfxWriteLogger(HANDLE hLogger, LPCTSTR formater, ...)
{
	if (hLogger == NULL)
		return ;

	CLogger* pLogger = reinterpret_cast<CLogger*>(hLogger);
	if (pLogger != NULL)
	{
		TCHAR xbuf[8192] = {0};
		int nLen = 0, nOffset = 0;

		SYSTEMTIME st = {0};
		GetLocalTime(&st);
		nLen = _stprintf_s(&xbuf[nLen], _countof(xbuf) - nLen - 1, TEXT("%02d:%02d:%02d "), st.wHour, st.wMinute, st.wSecond);
		nOffset = nLen;

		va_list marker;
		va_start(marker, formater);
		nLen += _vstprintf_s(&xbuf[nLen], _countof(xbuf) - nLen - 1, formater, marker);
		va_end(marker);

		xbuf[nLen++] = TEXT('\r');
		xbuf[nLen++] = TEXT('\n');
		xbuf[nLen] = 0;
		pLogger->WriteLog(xbuf, nLen);
		_tprintf_s(&xbuf[nOffset]);
	}
}

void  __stdcall AfxCloseLogger(HANDLE& hLogger)
{
	CLogger* pLogger = reinterpret_cast<CLogger*>(hLogger);
	if (pLogger != NULL)
	{
		delete pLogger;
		pLogger = NULL;
	}
	hLogger = NULL;
}

void __cdecl AfxWriteDbgLog(LPCTSTR formater, ...)
{
	TCHAR log[0x1000] = {0};

	va_list marker;
	va_start(marker, formater);
	_vstprintf_s(log, _countof(log), formater, marker);
	va_end(marker);

	OutputDebugString(TEXT("[bf:ygx]:"));
	OutputDebugString(log);
	OutputDebugString(TEXT("\r\n"));
}

END_NAMESPACE_KCORE