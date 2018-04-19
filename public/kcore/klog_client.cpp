#include "stdafx.h"
#include "klog_client.h"
#include <time.h>

START_NAMESPACE_KCORE

Cklog_client::Cklog_client(Log_Handler_Fun log_out)
:m_Out_Loghandle_Fun(log_out)
{
	m_socket = INVALID_SOCKET;
}


Cklog_client::~Cklog_client()
{
}

bool Cklog_client::start(const std::wstring &ip, USHORT port)
{
	m_socket = AfxSocketConnect(ip.c_str(), port);
	if (INVALID_SOCKET == m_socket)
	{
		m_Out_Loghandle_Fun(LogLevel::err, L"connect server fail");
		return false;
	}
	return true;
}

void Cklog_client::stop()
{
	if (m_socket != INVALID_SOCKET)
		AfxSocketClose(m_socket);
}

void Cklog_client::log(LogLevel level, const std::wstring &msg)
{
	TCHAR szBuffer[MAX_PATH] = { 0 };
	AfxFormatTimeString(_time32(NULL), szBuffer, MAX_PATH);

	std::wstring buffer = std::wstring(szBuffer) + L" [" + log::to_wstr(level) + L"] " + msg;

#ifndef _DEBUG
	if (level > LogLevel::debug)
#endif
		m_Out_Loghandle_Fun(level, std::cref(msg));

	if (m_socket != INVALID_SOCKET && level >= LogLevel::info)
	{
		char szBuffer[1024] = { 0 }, szRecv[1024] = { 0 };
		DWORD length = 1024;
		CPkgHelper pkgSend(szBuffer, CMD_CLI_LOGTOSVR,0);
		pkgSend << (DWORD)0 << (DWORD)level << buffer;
		AfxSocketExecute(m_socket, szBuffer, pkgSend.GetLength(), szRecv, length);
	}
}


END_NAMESPACE_KCORE