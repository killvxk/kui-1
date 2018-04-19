#include "OpenHMI.h"

#ifdef OS_WIN
	//#include <process.h>
#endif

#ifdef OS_QNX
	
#endif

#ifdef OS_LINUX

#endif

namespace OpenHMI
{
#ifdef OS_WIN
	CAutoSocket::CAutoSocket(String &strHost, INT iPort)
		: m_pListener(NULL)
	{
		char acHost[32];
		String::convertToMBCS(strHost, acHost, 32);

		m_tagServer_Addr.sin_family = AF_INET;
		m_tagServer_Addr.sin_addr.s_addr = inet_addr(acHost);
		m_tagServer_Addr.sin_port = htons(iPort);
		memset(m_tagServer_Addr.sin_zero, 0x00, 8);
	}

	CAutoSocket:: ~CAutoSocket()
	{
		closeSocket();
	}

	BOOLEAN	CAutoSocket::initSocket()
	{
		CLog::info(MODULE_NAME, L"Socket begin to initialize.");

		WSADATA  Ws;
		if (WSAStartup(MAKEWORD(2,2), &Ws) != 0)
		{
			return FALSE;
		}

		m_socketClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socketClient == INVALID_SOCKET)
		{
			CLog::info(MODULE_NAME, L"Socket fail to initialize.");
			return FALSE;
		}
		else
		{
			CLog::info(MODULE_NAME, L"Socket successfully initialized.");	
			return TRUE;
		}
	}

	BOOLEAN CAutoSocket::connectSocket()
	{
		CLog::info(MODULE_NAME, L"Socket begin to connect.");

		INT iTryCount = 0;
		while ((connect(m_socketClient, (struct sockaddr *)&m_tagServer_Addr, sizeof(m_tagServer_Addr)) !=  SOCKET_ERROR))
		{
			iTryCount++;

			if (iTryCount == 5)
			{
				CLog::info(MODULE_NAME, L"Socket fail to connect.");
				return FALSE;
			}

			Sleep(1000);
		}

		CLog::info(MODULE_NAME, L"Socket successfully connected.");
		return TRUE;
	}

	void CAutoSocket::closeSocket()
	{
		closesocket(m_socketClient);
		WSACleanup();
	}

	void CAutoSocket::start()
	{
		BYTE abtBuffer[25];
		INT	 iSize = 0;
		while (TRUE)
		{
			//if (FD_ISSET(m_iSockfd, &m_readFds))
			//{
				if ((iSize = recv(m_socketClient, (char*)&abtBuffer, sizeof(abtBuffer) + 1, 0)) == -1)
				{
					Sleep(20);
					continue;
				}
				else
				{
					if (m_pListener != NULL)
					{
						m_pListener->onReceiveData(iSize, abtBuffer, this);
					}					
				}
			//}
		}
	}	

	INT CAutoSocket::sendData(INT iSize, const BYTE * pbtData)
	{
		return send(m_socketClient, (const char*)pbtData, iSize, 0);
	}
#endif

#ifdef OS_QNX
	CAutoSocket::CAutoSocket(String &strHost, INT iPort)
		: m_pListener(NULL)
		, m_iSockfd(-1)
	{
		char acHost[32];
		String::convertToMBCS(strHost, acHost, 32);
		
		m_tagServer_addr.sin_family = AF_INET;
		m_tagServer_addr.sin_port = htons(iPort);
		m_tagServer_addr.sin_addr.s_addr = inet_addr(acHost);
		bzero(&(m_tagServer_addr.sin_zero),sizeof(m_tagServer_addr.sin_zero));
	}

	CAutoSocket:: ~CAutoSocket()
	{
		closeSocket();
	}

	BOOLEAN	CAutoSocket::initSocket()
	{
		CLog::info(MODULE_NAME, L"Socket begin to initialize.");

		m_iSockfd = socket( AF_INET, SOCK_STREAM, 0);

		CLog::info(MODULE_NAME, L"Socket successfully initialized.");

		return TRUE;
	}
	
	BOOLEAN CAutoSocket::connectSocket()
	{
		CLog::info(MODULE_NAME, L"Socket begin to connect.");

		INT iTryCount = 0;
		while ((connect(m_iSockfd, (struct sockaddr *)&m_tagServer_addr, sizeof(struct sockaddr)) != 0))
		{
			iTryCount++;

			if (iTryCount == 5)
			{
				CLog::info(MODULE_NAME, L"Socket fail to connect.");
				return FALSE;
			}

			delay(1000);
		}

		fcntl(m_iSockfd, F_SETFL, O_NONBLOCK);

		CLog::info(MODULE_NAME, L"Socket successfully connected.");
		return TRUE;
	}

	void CAutoSocket::closeSocket()
	{
		close(m_iSockfd);
		m_iSockfd = -1;
	}

	void CAutoSocket::start()
	{
		BYTE abtBuffer[25];
		
		while (TRUE)
		{
			INT	 iSize = 0;
			FD_ZERO(&m_readFds);
			FD_SET(m_iSockfd, &m_readFds);
			select(m_iSockfd+1, &m_readFds, NULL, NULL, NULL);
			if (FD_ISSET(m_iSockfd, &m_readFds))
			{
				if ((iSize = recv(m_iSockfd, &abtBuffer, sizeof(abtBuffer), 0)) == -1)
				{
					delay(1);
					continue;
				}
				else
				{
					if (m_pListener != NULL)
					{
						m_pListener->onReceiveData(iSize, abtBuffer, this);
					}
				}
			}			
		}
	}

	INT CAutoSocket::sendData(INT iSize, const BYTE * pbtData)
	{
		return send(m_iSockfd, pbtData, iSize, 0);
	}

#endif

#ifdef OS_LINUX
	CAutoSocket::CAutoSocket(String &strHost, INT iPort)
		: m_pListener(NULL)
		, m_iSockfd(-1)
	{
		char acHost[32];
		String::convertToMBCS(strHost, acHost, 32);

		m_iSockfd = socket( AF_INET, SOCK_STREAM, 0);
		m_tagServer_addr.sin_family = AF_INET;
		m_tagServer_addr.sin_port = htons(iPort);
		m_tagServer_addr.sin_addr.s_addr = inet_addr(acHost);
		bzero(&(m_tagServer_addr.sin_zero),sizeof(m_tagServer_addr.sin_zero));
	}
	
	CAutoSocket:: ~CAutoSocket()
	{
		closeSocket();
	}

	BOOLEAN	CAutoSocket::initSocket()
	{
		CLog::info(MODULE_NAME, L"Socket begin to initialize.");

		m_iSockfd = socket( AF_INET, SOCK_STREAM, 0);

		CLog::info(MODULE_NAME, L"Socket successfully initialized.");

		return TRUE;
	}

	BOOLEAN CAutoSocket::connectSocket()
	{
		CLog::info(MODULE_NAME, L"Socket begin to connect.");

		INT iTryCount = 0;
		while ((connect(m_iSockfd, (struct sockaddr *)&m_tagServer_addr, sizeof(struct sockaddr)) != 0))
		{
			iTryCount++;

			if (iTryCount == 5)
			{
				CLog::info(MODULE_NAME, L"Socket fail to connect.");
				return FALSE;
			}

			usleep(1000);
		}
		fcntl(m_iSockfd, F_SETFL, O_NONBLOCK);

		CLog::info(MODULE_NAME, L"Socket successfully connected.");
		return TRUE;
	}

	void CAutoSocket::closeSocket()
	{
		close(m_iSockfd);
	}

	void CAutoSocket::start()
	{
		BYTE abtBuffer[25];
		INT	 iSize = 0;
		while (TRUE)
		{
			FD_ZERO(&m_readFds);
			FD_SET(m_iSockfd, &m_readFds);
			select(m_iSockfd+1, &m_readFds, NULL, NULL, NULL);
			if (FD_ISSET(m_iSockfd, &m_readFds))
			{
				if ((iSize = recv(m_iSockfd, &abtBuffer, sizeof(abtBuffer), 0)) == -1)
				{
					usleep(50);
					continue;
				}
				else
				{
					if (m_pListener != NULL)
					{
						m_pListener->onReceiveData(iSize, abtBuffer, this);
					}
				}
			}
		}
	}

	INT CAutoSocket::sendData(INT iSize, const BYTE * pbtData)
	{
		return send(m_iSockfd, pbtData, iSize, 0);
	}

#endif
}