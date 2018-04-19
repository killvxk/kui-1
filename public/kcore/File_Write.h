#pragma once
#include "public.h"

START_NAMESPACE_KCORE

struct content_t
{
	DWORD_PTR id;
	LogLevel level;
	TCHAR szBuffer[1024];
};

class CFile_Write : public LogHandle
{
public:
	CFile_Write();
	~CFile_Write();

	virtual void AddId(DWORD_PTR id);
	virtual void EraseId(DWORD_PTR id);
	virtual void Write(DWORD_PTR id, LogLevel level, TCHAR* pMsg);
	virtual void Flush();
	virtual void Release();
	static UINT __stdcall RunThreadEx(LPVOID lpVoid);
private:
	void	     OnWrite();
	content_t    PopTask();
	void		 PushTask(content_t& content);
private:
	std::wstring	   m_strAppPath;
	CLock			   m_lockClient;
	map<DWORD_PTR, HANDLE> m_mapClientInfo;
	CLock			   m_lockQueue;
	queue<content_t>   m_taskQueue;
	HANDLE			   m_hSemaphore;
	HANDLE			   m_hExited;
	HANDLE			   m_hThread;
};

END_NAMESPACE_KCORE