#pragma once  
#ifndef imcp_H_H_H
#define imcp_H_H_H

#include "stdafx.h"
#include "ping.h"
#include <iostream>
#include <thread>
#include <string>
#include <windows.h>


class imcpthread
{

public:
	imcpthread()
	{	
		th = NULL;
		this->exit = ::CreateEvent(NULL, FALSE, FALSE, NULL);	
	}
	void run()
	{
		th = new std::thread(&imcpthread::domain,this);
		
	}
	int domain()
	{	
		UINT start =0;
		char SendBuffer[PKG_MAXLENGTH] = { 0 }, RecvBuffer[PKG_MAXLENGTH] = { 0 };
		CPkgHelper pkg(SendBuffer, CMD_CLI_CLIENT2CONSOLE, OBJECT_CLIENT);
		pkg << CONTROL_CLINET_PING;
		char send[0x100] = { 0 };
		sprintf_s(send, 0x100,"ping %s",cmdline);	
		pkg << send;
		AfxSocketExecuteLock(gSocket, SendBuffer, pkg.GetLength(), RecvBuffer, PKG_MAXLENGTH);
		
		CPing objPing;
		PingReply reply;;
		while (true)
		{
			if ( start++ > 1000 * 60 * 20) //不停止最多跑20分钟
				return 0;
			HANDLE hEvents[] = { ghExited, exit };
			if (::WaitForMultipleObjects(_countof(hEvents), hEvents, FALSE, 1000) != WAIT_TIMEOUT)
				return 0;
			char ipstr[64] = { 0 };
			BOOL ret=objPing.Ping(cmdline, ipstr, &reply);
			char recv[0x100] = { 0 };
			if (ret)		
				sprintf_s(recv,0x100, "Reply from %s: bytes=%d time=%ldms TTL=%ld", ipstr, reply.m_dwBytes, reply.m_dwRoundTripTime, reply.m_dwTTL);
			else
				sprintf_s(recv, 0x100,"error:%d", GetLastError());
			CPkgHelper pkg(SendBuffer, CMD_CLI_CLIENT2CONSOLE, OBJECT_CLIENT);
			pkg << CONTROL_CLINET_PING;
			pkg << recv;
			AfxSocketExecuteLock(gSocket, SendBuffer, pkg.GetLength(), RecvBuffer, PKG_MAXLENGTH);
			Sleep(1000);
		}

		return 0;
	};
	void stop()
	{
		SetEvent(this->exit);
		if (th&&th->joinable())
		{
			th->join();
			delete th;
			th = NULL;
		}	
		if (exit)
		{
			CloseHandle(this->exit);
			exit = NULL;
		}

	
	}
	void reset(std::wstring cmd)
	{
		stop();	
		this->exit = ::CreateEvent(NULL, FALSE, FALSE, NULL);	
		strcpy_s(cmdline, MAX_PATH, _bstr_t(cmd.c_str()));
		
	}

private:
	HANDLE  exit;
	char cmdline[MAX_PATH];	
	std::thread* th;
};




#endif
