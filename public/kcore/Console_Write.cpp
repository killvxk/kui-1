#include "stdafx.h"
#include "Console_Write.h"
#include <atlbase.h>

START_NAMESPACE_KCORE

CConsole_Write::CConsole_Write(std::string strip, USHORT nPort)
{
	m_strIp = strip;
	m_nPort = nPort;
	m_udpSock = INVALID_SOCKET;
}


CConsole_Write::~CConsole_Write()
{
	if (m_udpSock != INVALID_SOCKET)
	{
		closesocket(m_udpSock);
		m_udpSock = INVALID_SOCKET;
	}
}

void CConsole_Write::AddId(DWORD_PTR id)
{

}

void CConsole_Write::EraseId(DWORD_PTR id)
{

}

void CConsole_Write::Write(DWORD_PTR id, LogLevel level, TCHAR* pMsg)
{
	if (INVALID_SOCKET == m_udpSock)
		m_udpSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_udpSock)
		return;

	sockaddr_in addrIp[2] = { 0 };
	int addrlen = sizeof(addrIp);
	getpeername((SOCKET)id, (PSOCKADDR)&addrIp, &addrlen);
	TCHAR szClientIp[MAX_PATH] = { 0 };
	AfxFormatIpString(addrIp->sin_addr.S_un.S_addr, szClientIp, MAX_PATH);

	char buf[PKG_MAXLENGTH] = { 0 };
	CPkgHelper pkg(buf, CMD_CON_NOTIFYLOGINFO, OBJECT_UNKOWN);
	pkg << LOG_TYPE_SSD << szClientIp << pMsg;

	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_nPort);
	addr.sin_addr.s_addr = inet_addr(m_strIp.c_str());
	int length = pkg.GetLength();
	DWORD left = length;
	while (left)
	{
		int len = sendto(m_udpSock, &buf[length - left], (left >= 0x10000 ? 0x10000 : left),
			0, (PSOCKADDR)&addr, sizeof(addr));
		if (len <= 0)
			break;
		left -= len;
	}
}

void CConsole_Write::Flush()
{

}

void CConsole_Write::Release()
{

}

END_NAMESPACE_KCORE