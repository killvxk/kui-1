#include "stdafx.h"
#include "File_Write.h"
#include <process.h>

START_NAMESPACE_KCORE

CFile_Write::CFile_Write()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	AfxGetAppPath(szPath, MAX_PATH, _T("log\\client\\"));
	m_strAppPath = szPath;

	m_hSemaphore = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	m_hExited = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, RunThreadEx, this, 0, NULL);
}


CFile_Write::~CFile_Write()
{
}

void CFile_Write::AddId(DWORD_PTR id)
{
	DWORD dwEnableClientLog = AfxGetSysLong(_T("config"), _T("enableclientlog"), 0);
	if (dwEnableClientLog == 0)
		return;

	AfxCreateDirectory(m_strAppPath.c_str());
	sockaddr_in addrIp[2] = { 0 };
	int addrlen = sizeof(addrIp);
	if (SOCKET_ERROR == getpeername((SOCKET)id, (PSOCKADDR)&addrIp, &addrlen))
		return;
	TCHAR szClientIp[MAX_PATH] = { 0 };
	AfxFormatIpString(addrIp->sin_addr.S_un.S_addr, szClientIp, MAX_PATH);
	std::wstring strFile = m_strAppPath + szClientIp + _T(".log");
	HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return ;

	//write unicode file sig.
#ifdef _UNICODE
	short start = 0;
	DWORD dwBytes = 0;
	if (!ReadFile(hFile, &start, 2, &dwBytes, NULL) || start != 0xfeff)
	{
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		start = (short)0xfeff;
		WriteFile(hFile, &start, sizeof(start), &dwBytes, NULL);
	}
#endif
	SetFilePointer(hFile, 0, 0, FILE_END);

	OBJECT_LOCK(m_lockClient);
	m_mapClientInfo.insert(std::make_pair(id, hFile));
}

void CFile_Write::EraseId(DWORD_PTR id)
{
	OBJECT_LOCK(m_lockClient);
	map<DWORD_PTR, HANDLE>::iterator itr = m_mapClientInfo.find(id);
	if (itr != m_mapClientInfo.end())
	{
		if (itr->second)
			CloseHandle(itr->second);
		m_mapClientInfo.erase(itr);
	}
}

void CFile_Write::Write(DWORD_PTR id, LogLevel level, TCHAR* pMsg)
{
	content_t content = { 0 };
	content.id = id;
	content.level = level;
	_tcscpy_s(content.szBuffer, pMsg);
	lstrcat(content.szBuffer, _T("\r\n"));
	PushTask(content);
}

void CFile_Write::Flush()
{

}

UINT __stdcall CFile_Write::RunThreadEx(LPVOID lpVoid)
{
	CFile_Write *pThis = reinterpret_cast<CFile_Write*>(lpVoid);
	if (pThis)
		pThis->OnWrite();
	return 0;
}

void CFile_Write::OnWrite()
{
	HANDLE	hEventArray[2] = { m_hExited, m_hSemaphore };
	DWORD dwWaitResult = 0;
	while (true)
	{
		dwWaitResult = WaitForMultipleObjects(2, hEventArray, FALSE, INFINITE);
		if (dwWaitResult != WAIT_OBJECT_0 + 1)
			break;

		content_t content = PopTask();

		OBJECT_LOCK(m_lockClient);
		map<DWORD_PTR, HANDLE>::iterator itr = m_mapClientInfo.find(content.id);
		if (itr != m_mapClientInfo.end())
		{
			if (itr->second)
			{
				DWORD dwWriteBytes = 0;
				int nLen = lstrlen(content.szBuffer);
				nLen *= sizeof(TCHAR);
				WriteFile(itr->second, content.szBuffer, nLen, &dwWriteBytes, NULL);
			}
		}
	}
}

void CFile_Write::Release()
{
	if (m_hExited)
		SetEvent(m_hExited);

	if (m_hThread)
		WaitForSingleObject(m_hThread, INFINITE);

	if (m_hExited != NULL)
	{
		::CloseHandle(m_hExited);
		m_hExited = NULL;
	}

	if (m_hSemaphore != NULL)
	{
		::CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
	}
	if (m_hThread)
	{
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

void CFile_Write::PushTask(content_t& content)
{
	OBJECT_LOCK(m_lockQueue);
	m_taskQueue.push(content);
	ReleaseSemaphore(m_hSemaphore, 1, NULL);
}

content_t CFile_Write::PopTask()
{
	OBJECT_LOCK(m_lockQueue);
	content_t content = m_taskQueue.front();
	m_taskQueue.pop();
	return content;
}


END_NAMESPACE_KCORE