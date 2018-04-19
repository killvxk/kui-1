#include "stdafx.h"
#include <process.h>
#include <time.h>
#include <MSWSock.h>
#include <MSTcpIP.h>
#include <map>

START_NAMESPACE_KCORE

typedef enum CALLBACK_EVENT
{
	CALLBACK_ACCEPT		= 0,
	CALLBACK_RECEIVE	= 1,
	CALLBACK_SEND		= 2,
	CALLBACK_CLOSE		= 3
}CALLBACK_EVENT;

typedef struct AyncResult : public OVERLAPPED
{
	SOCKET		socket;
	DWORD		peerip;
	__time32_t	acktime;
	
	CALLBACK_EVENT event;
	WSABUF  buffer;
	DWORD	totallength;
	DWORD	sendlength;
	DWORD	recvlength;
	char	sendbuf[PKG_MAXLENGTH];
	char	recvbuf[PKG_MAXLENGTH];
}AYNCRESULT, * LPAYNCRESULT;

class CNetModule : public INetModule
{
public:
	CNetModule();
	~CNetModule();
public:
	virtual void Release() { delete this; }
	virtual DWORD Start(WORD port, DWORD_PTR reserved = 0);
	virtual void Stop();
	virtual unsigned short GetListPort() { return m_nPort; }
	virtual void SetNetCallback(INetCallback* pNetCallback) { m_pNetCallback = pNetCallback; }
	virtual bool GetPeerInfo(DWORD_PTR Id, SOCKET& sck, DWORD& peerip);
private:
	DWORD StartListenSocket();
	DWORD ParasePackage(const char* pPackage, long length);
	DWORD PostAcceptRequest();
	void  PostSendReqesut(DWORD_PTR key, LPAYNCRESULT pAyncResult, bool bContiue, DWORD length);
	void  PostRecvRequest(DWORD_PTR key, LPAYNCRESULT pAyncResult, bool bContiue, DWORD length);
	void  OnAccpet(DWORD_PTR key,  LPAYNCRESULT pAyncResult);
	void  OnRecevie(DWORD_PTR key, LPAYNCRESULT pAyncResult, DWORD length);
	void  OnSend(DWORD_PTR key,    LPAYNCRESULT pAyncResult, DWORD length);
	void  OnClose(DWORD_PTR key,   LPAYNCRESULT pAyncResult);
private:
	static unsigned __stdcall AcceptThread(void* pVoid);
	static unsigned __stdcall WorkThread(void* pVoid);
private:
	INetCallback* m_pNetCallback;
	HANDLE m_hIocp;
	unsigned short m_nPort;
	SOCKET m_sckListen;
	HANDLE m_hExited;
	LPFN_ACCEPTEX m_lpfnAcceptEx;
	std::vector<HANDLE> m_Threads;
	CLock m_lockClient;
	std::map<SOCKET, LPAYNCRESULT> m_mapClient;
};

CNetModule::CNetModule()
{
	m_pNetCallback = NULL;
	m_hIocp = NULL;
	m_nPort = 0;;
	m_sckListen = INVALID_SOCKET;
	m_hExited = NULL;
	m_lpfnAcceptEx = NULL;
}

CNetModule::~CNetModule()
{
	Stop();
}

DWORD CNetModule::Start(WORD port, DWORD_PTR reserved/* = 0*/)
{
	m_nPort = port;
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_hIocp == NULL)
		return GetLastError();

	long dwError = StartListenSocket();
	if (dwError != ERROR_SUCCESS)
	{
		Stop();
		return dwError;
	}

	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, this, 0, NULL);
	m_Threads.push_back(hThread);

	SYSTEM_INFO info = {0};
	GetSystemInfo(&info);
	for (DWORD idx=0; idx<info.dwNumberOfProcessors * 2; idx++)
	{
		hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
		m_Threads.push_back(hThread);
	}
	return ERROR_SUCCESS;
}

void CNetModule::Stop()
{
	if (m_sckListen != INVALID_SOCKET)
	{
		closesocket(m_sckListen);
		m_sckListen = INVALID_SOCKET;
	}
	if (m_hExited != NULL && m_Threads.size())
	{
		SetEvent(m_hExited);
		for (size_t idx=0; idx<m_Threads.size(); idx++)
		{
			PostQueuedCompletionStatus(m_hIocp, 0, 0, (LPOVERLAPPED)-1);
		}
		WaitForMultipleObjects((DWORD)m_Threads.size(), &m_Threads[0], TRUE, INFINITE);
		for (size_t idx=0; idx<m_Threads.size(); idx++)
		{
			CloseHandle(m_Threads[idx]);
		}
		m_Threads.clear();
	}

	if (m_hIocp != NULL)
	{
		CloseHandle(m_hIocp);
		m_hIocp = NULL;
	}

	if (m_hExited != NULL)
	{
		CloseHandle(m_hExited);
		m_hExited = NULL;
	}

	for (std::map<SOCKET, LPAYNCRESULT>::iterator it = m_mapClient.begin(); it != m_mapClient.end(); it ++)
	{
		try
		{
			if (it->second->socket != INVALID_SOCKET)
				closesocket(it->second->socket);
		}
		catch (...) {}
	}
	m_mapClient.clear();
}

bool CNetModule::GetPeerInfo(DWORD_PTR Id, SOCKET& sck, DWORD& peerip)
{
	OBJECT_LOCK(m_lockClient);
	std::map<SOCKET, LPAYNCRESULT>::const_iterator it = m_mapClient.find(Id);
	if (it != m_mapClient.end())
	{
		sck = it->second->socket;
		peerip = it->second->peerip;
		return true;
	}
	return false;
}

DWORD CNetModule::StartListenSocket()
{
	DWORD dwError = ERROR_SUCCESS;
	GUID  GuidAcceptEx = WSAID_ACCEPTEX;
	DWORD dwBytes = 0;
	sockaddr_in addr = {0};

	try
	{
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_hExited == NULL)
			throw GetLastError();

		m_sckListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_sckListen == INVALID_SOCKET)
			throw GetLastError();

		addr.sin_family = AF_INET;
		addr.sin_port = htons(m_nPort);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (SOCKET_ERROR == bind(m_sckListen, (PSOCKADDR)&addr, sizeof(addr)))
			throw GetLastError();

		if (SOCKET_ERROR == listen(m_sckListen, SOMAXCONN))
			throw GetLastError();

		if (SOCKET_ERROR == WSAIoctl(m_sckListen, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, 
			sizeof(GuidAcceptEx), &m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx), &dwBytes, NULL, NULL))
			throw GetLastError();

		if (NULL == CreateIoCompletionPort((HANDLE)m_sckListen, m_hIocp, m_sckListen, 0))
			throw GetLastError();
	}
	catch (DWORD dwError)
	{
		if (m_sckListen != INVALID_SOCKET)
		{
			closesocket(m_sckListen);
			m_sckListen = INVALID_SOCKET;
		}
		if (m_hExited != NULL)
		{
			CloseHandle(m_hExited);
			m_hExited = NULL;
		}
		return dwError;
	}

	return ERROR_SUCCESS;
}

//disconnect if return 0.
DWORD CNetModule::ParasePackage(const char* pPackage, long length)
{
	if (length < sizeof(PkgHeader))
		return PKG_MAXLENGTH;	//sizeof(PkgHeader);

	PkgHeader* pkgheader = (PkgHeader*)pPackage;
	if (pkgheader->sig != PKG_SIG|| pkgheader->length >= PKG_MAXLENGTH)
		return 0;	//for close.
	return pkgheader->length;
}

DWORD CNetModule::PostAcceptRequest()
{
	for(int idx=0; idx<10; idx++)
	{
		SOCKET AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if(INVALID_SOCKET == AcceptSocket)
		{
			DWORD dwError = WSAGetLastError();
			return dwError;
		}
		DWORD dwByte;
		LPAYNCRESULT pAyncResult = new AYNCRESULT;
		ZeroMemory(pAyncResult, sizeof(AYNCRESULT));
		pAyncResult->socket = AcceptSocket;
		pAyncResult->event  = CALLBACK_ACCEPT;
		pAyncResult->acktime = 0;
		if(!m_lpfnAcceptEx(m_sckListen, AcceptSocket, (PVOID)&pAyncResult->recvbuf, 0, 
			sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwByte, pAyncResult))
		{
			DWORD dwError = WSAGetLastError();
			if(ERROR_IO_PENDING != dwError)
			{
				::closesocket(AcceptSocket);
				return dwError;
			}
		}
	}
	return ERROR_SUCCESS;
}

void CNetModule::PostSendReqesut(DWORD_PTR key, LPAYNCRESULT pAyncResult, bool bContiue, DWORD length)
{
	if (bContiue)	//发送本包剩余的数据
	{
		pAyncResult->totallength -= length;
		pAyncResult->sendlength  += length;
		pAyncResult->buffer.buf   = pAyncResult->sendbuf + pAyncResult->sendlength;
		pAyncResult->buffer.len	  = pAyncResult->totallength - pAyncResult->sendlength;
	}
	else			//开始发新的数据包。
	{
		pAyncResult->totallength = length;
		pAyncResult->sendlength  = 0;
		pAyncResult->buffer.buf  = pAyncResult->sendbuf;
		pAyncResult->buffer.len  = length;
		pAyncResult->recvlength  = 0;
	}

	DWORD dwSendBytes = 0;
	ZeroMemory((LPOVERLAPPED)pAyncResult, sizeof(OVERLAPPED));
	pAyncResult->event = CALLBACK_SEND;
	int nRet = WSASend(pAyncResult->socket, &pAyncResult->buffer, 1, &dwSendBytes, 0, pAyncResult, NULL);
	if (nRet == SOCKET_ERROR)
	{
		DWORD dwRet = WSAGetLastError();
		if (WSA_IO_PENDING != dwRet)
		{
			OnClose(key, pAyncResult);
		}
	}
}

void CNetModule::PostRecvRequest(DWORD_PTR key, LPAYNCRESULT pAyncResult, bool bContiue, DWORD length)
{
	if (bContiue)	//继续接收数据包尾部数据
	{
		pAyncResult->recvlength += length;
		pAyncResult->buffer.buf += length;
		pAyncResult->buffer.len -= length;
	}
	else			//开始接收一个新的数据包
	{
		pAyncResult->buffer.buf = pAyncResult->recvbuf;
		pAyncResult->buffer.len = sizeof(pAyncResult->recvbuf);
		pAyncResult->recvlength = 0;
		pAyncResult->sendlength = pAyncResult->totallength = 0;
	}

	DWORD dwRecvBytes = 0;
	DWORD dwFlag = 0;
	ZeroMemory((LPOVERLAPPED)pAyncResult, sizeof(OVERLAPPED));
	pAyncResult->event = CALLBACK_RECEIVE;
	int nRet = WSARecv(pAyncResult->socket, &pAyncResult->buffer, 1, &dwRecvBytes, &dwFlag, 
		(LPOVERLAPPED)pAyncResult, NULL);
	if (nRet == SOCKET_ERROR)
	{
		DWORD dwRet = WSAGetLastError();
		if (WSA_IO_PENDING != dwRet)
		{
			OnClose(key, pAyncResult);
		}
	}
}

void  CNetModule::OnAccpet(DWORD_PTR key,  LPAYNCRESULT pAyncResult)
{
	//set socket option.
	DWORD dwBytes = 0;
	tcp_keepalive sKA_Settings = {0}, sReturned = {0};
	sKA_Settings.onoff = 1;
	sKA_Settings.keepalivetime = 5000;
	sKA_Settings.keepaliveinterval = 3000;
	WSAIoctl(pAyncResult->socket, SIO_KEEPALIVE_VALS, &sKA_Settings, sizeof(sKA_Settings), 
		&sReturned, sizeof(sReturned), &dwBytes, NULL, NULL);

	linger InternalLinger = { 1, 0 };
	setsockopt(pAyncResult->socket, SOL_SOCKET, SO_LINGER, (const char*)&InternalLinger, sizeof(linger));

	//handle connect event.
	{
		OBJECT_LOCK(m_lockClient);
		m_mapClient.insert(std::make_pair(pAyncResult->socket, pAyncResult));
		sockaddr_in addr[2] = {0};
		int addrlen = sizeof(addr)*sizeof(sockaddr_in);
		getpeername(pAyncResult->socket, (PSOCKADDR)&addr, &addrlen);
		pAyncResult->peerip	= addr[0].sin_addr.s_addr;
		pAyncResult->acktime = _time32(NULL);
		CreateIoCompletionPort((HANDLE)pAyncResult->socket, m_hIocp, (DWORD)pAyncResult->socket, 0);
		if (m_pNetCallback != NULL && ERROR_SUCCESS != m_pNetCallback->OnAccept(this, key))
		{
			OnClose(key, pAyncResult);
			return ;
		}
	}
	//post receive first request.
	PostRecvRequest(key, pAyncResult, false, 0);
}

void  CNetModule::OnRecevie(DWORD_PTR key, LPAYNCRESULT pAyncResult, DWORD length)
{
	pAyncResult->acktime = _time32(NULL);
	DWORD pkglength = ParasePackage(pAyncResult->recvbuf, pAyncResult->recvlength + length);

	//如果数据包有问题，或者接收到多余一包的数据，则主动关闭连接。
	if (pkglength == 0 || pkglength < pAyncResult->recvlength + length) //???
	{
		OnClose(key, pAyncResult);
		return ;
	}

	//接收到一个完整的数据包
	if (pkglength == pAyncResult->recvlength + length)
	{
		if (m_pNetCallback != NULL)
		{
			if (ERROR_SUCCESS != m_pNetCallback->OnReceive(this, key, pAyncResult->recvbuf, pAyncResult->sendbuf))
			{
				OnClose(key, pAyncResult);
			}
			else
			{
				PkgHeader* pkg = reinterpret_cast<PkgHeader*>(pAyncResult->sendbuf);
				PostSendReqesut(key, pAyncResult, false, pkg->length);
			}
		}
	}
	//本包还没有接收完成
	else if (pkglength > pAyncResult->recvlength + length)
	{
		PostRecvRequest(key, pAyncResult, true, length);
	}
}

void  CNetModule::OnSend(DWORD_PTR key, LPAYNCRESULT pAyncResult, DWORD length)
{
	pAyncResult->acktime = _time32(NULL);

	//发送完成
	if (pAyncResult->sendlength + length >= pAyncResult->totallength)
	{
		if (m_pNetCallback != NULL && ERROR_SUCCESS != m_pNetCallback->OnSend(this, key))
		{
			OnClose(key, pAyncResult);
			return ;
		}
		PostRecvRequest(key, pAyncResult, false, 0);
	}
	else	//继续发送未完成的包
	{
		PostSendReqesut(key, pAyncResult, true, length);
	}
}

void  CNetModule::OnClose(DWORD_PTR key,   LPAYNCRESULT pAyncResult)
{
	bool bRelease = false;
	{
		OBJECT_LOCK(m_lockClient);
		std::map<SOCKET, LPAYNCRESULT>::iterator it = m_mapClient.find(pAyncResult->socket);
		if (it != m_mapClient.end() && it->second == pAyncResult)
		{
			bRelease = true;
			m_mapClient.erase(it);
		}
	}
	if (bRelease)
	{
		if (m_pNetCallback != NULL)
			m_pNetCallback->OnClose(this, key);

		shutdown(pAyncResult->socket, SD_BOTH);
		closesocket(pAyncResult->socket);
	}
	pAyncResult->socket = INVALID_SOCKET;
	delete pAyncResult;
	pAyncResult = NULL;
}

unsigned __stdcall CNetModule::AcceptThread(void* pVoid)
{
	CNetModule* pThis = reinterpret_cast<CNetModule*>(pVoid);
	HANDLE hAcceptEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	WSAEventSelect(pThis->m_sckListen, hAcceptEvent, FD_ACCEPT);
	while (1)
	{
		HANDLE handle[] = {pThis->m_hExited, hAcceptEvent};
		DWORD  dwRet = WaitForMultipleObjects(2, handle, FALSE, INFINITE);
		if (dwRet == WAIT_OBJECT_0)
			break;
		else if (dwRet == WAIT_OBJECT_0 + 1)
			pThis->PostAcceptRequest();
		else
		{
			break;
		}
	}
	CloseHandle(hAcceptEvent);
	return 0;
}

unsigned __stdcall CNetModule::WorkThread(void* pVoid)
{
	CNetModule* pThis = reinterpret_cast<CNetModule*>(pVoid);
	DWORD        dwReadBytes = 0;
	DWORD_PTR	 key = 0;
	LPAYNCRESULT pAyncResult = NULL;

	while (1)
	{
		dwReadBytes = 0; key = NULL; pAyncResult = NULL;
		BOOL flag = ::GetQueuedCompletionStatus(pThis->m_hIocp,	&dwReadBytes, &key, (LPOVERLAPPED*)&pAyncResult, INFINITE);
		if (pAyncResult == (LPOVERLAPPED)-1 || pAyncResult == NULL)
			break;
		
		if (pAyncResult->event != CALLBACK_ACCEPT && dwReadBytes == 0)
		{
			pThis->OnClose(key, pAyncResult);
			continue ;
		}

		switch (pAyncResult->event)
		{
		case CALLBACK_ACCEPT:
			setsockopt(pAyncResult->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&(pThis->m_sckListen), sizeof(SOCKET));
			pThis->OnAccpet(pAyncResult->socket, pAyncResult);	//key is listen socket.
			break;
		case CALLBACK_RECEIVE:
			pThis->OnRecevie(key, pAyncResult, dwReadBytes);
			break;
		case CALLBACK_SEND:
			pThis->OnSend(key, pAyncResult, dwReadBytes);
			break;
		}
	}
	return ERROR_SUCCESS;
}

bool __stdcall AfxCreateINetModule(INetModule*& pINetModule)
{
	pINetModule = new CNetModule;
	return true;
}

END_NAMESPACE_KCORE