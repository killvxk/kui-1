/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

#include <winsock.h>

namespace OpenHMI
{
	class OPENHMI_API CAutoSocket : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CAutoSocket(String &strHost, INT iPort);
		virtual ~CAutoSocket();

		BOOLEAN	initSocket();
		BOOLEAN connectSocket();
		void	closeSocket();
		void	start();
		INT		sendData(INT iSize, const BYTE * pbtData);

		void	setListener(ISocketListener * pListener) { m_pListener = pListener; }	

	protected:// method		

	private:// method
		CAutoSocket(){}		
		CAutoSocket(const CAutoSocket&){}
		CAutoSocket& operator =(const CAutoSocket&){return *this;}

#ifdef OS_WIN
		
#endif

#ifdef OS_QNX
		
#endif

#ifdef OS_LINUX

#endif

	protected:// property
		

	private:// property	
		ISocketListener*		m_pListener;

#ifdef OS_WIN
		SOCKET					m_socketClient;
		struct sockaddr_in		m_tagServer_Addr;		
#endif

#ifdef OS_QNX
		INT 					m_iSockfd;
		fd_set 					m_readFds;
		struct sockaddr_in 		m_tagServer_addr;
#endif

#ifdef OS_LINUX
		INT		 				m_iSockfd;
		fd_set 					m_readFds;
		struct sockaddr_in 		m_tagServer_addr;
#endif

	};
}