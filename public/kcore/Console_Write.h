#pragma once
#include "public.h"

START_NAMESPACE_KCORE


class CConsole_Write : public LogHandle
{
public:
	CConsole_Write(std::string strip="127.0.0.1",USHORT nPort=CON_UDP_PORT);
	~CConsole_Write();

	virtual void AddId(DWORD_PTR id);
	virtual void EraseId(DWORD_PTR id);
	virtual void Write(DWORD_PTR id, LogLevel level, TCHAR* pMsg);
	virtual void Flush();
	virtual void Release();
private:
	std::string m_strIp;
	USHORT		m_nPort;
	SOCKET		m_udpSock;
};

END_NAMESPACE_KCORE