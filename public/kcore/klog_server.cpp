#include "stdafx.h"
#include "klog_server.h"
#include "File_Write.h"
#include "Console_Write.h"

START_NAMESPACE_KCORE

Cklog_server::Cklog_server()
{
}


Cklog_server::~Cklog_server()
{
}

Cklog_server& Cklog_server::GetInstance()
{
	static Cklog_server logServer;
	return logServer;
}

bool Cklog_server::StartKlogServer()
{
	if (m_pINetModule != NULL)
		return false;

	LogHandle *pFile_Write = new CFile_Write;
	LogHandle *pConsole_Write = new CConsole_Write;
	m_arLogHandle.push_back(pFile_Write);
	m_arLogHandle.push_back(pConsole_Write);

	if (!AfxCreateINetModule(m_pINetModule) || m_pINetModule == NULL)
		return false;

	m_pINetModule->SetNetCallback(this);
	if (m_pINetModule->Start(LOG_TCP_PORT_EX) != ERROR_SUCCESS)
		return false;

	return true;
}

void Cklog_server::StopKlogServer()
{
	if (m_pINetModule != NULL)
	{
		m_pINetModule->Stop();
		m_pINetModule->Release();
		m_pINetModule = NULL;
	}

	for (size_t i = 0; i < m_arLogHandle.size(); i++)
	{
		LogHandle* &pLogHandle = m_arLogHandle[i];
		if (pLogHandle != NULL)
		{
			delete pLogHandle;
			pLogHandle = NULL;
		}
	}
	m_arLogHandle.clear();
}

long Cklog_server::OnAccept(INetModule* pNetModule, DWORD_PTR Id)
{
	for (size_t i = 0; i < m_arLogHandle.size(); i++)
	{
		LogHandle* &pLogHandle = m_arLogHandle[i];
		if (pLogHandle != NULL)
			pLogHandle->AddId(Id);
	}
	return ERROR_SUCCESS;
}

long Cklog_server::OnSend(INetModule* pNetModule, DWORD_PTR Id)
{
	return ERROR_SUCCESS;
}

long Cklog_server::OnReceive(INetModule* pNetModule, DWORD_PTR Id, LPCSTR pRecvBuf, LPSTR pSendBuf)
{
	CPkgHelper pkgRecv(pRecvBuf);
	CPkgHelper pkgSend(pSendBuf, PKG_MAXLENGTH, pkgRecv.GetHeader()->command, pkgRecv.GetHeader()->type);
	switch (pkgRecv.GetHeader()->command)
	{
	case CMD_CLI_LOGTOSVR:  HandleClientLog(Id,pkgRecv);  break;
	default:break;
	}
	return ERROR_SUCCESS;
}

void Cklog_server::OnClose(INetModule* pNetModule, DWORD_PTR Id)
{
	for (size_t i = 0; i < m_arLogHandle.size(); i++)
	{
		LogHandle* &pLogHandle = m_arLogHandle[i];
		if (pLogHandle != NULL)
			pLogHandle->EraseId(Id);
	}
}

void Cklog_server::HandleClientLog(DWORD_PTR Id,CPkgHelper& In)
{
	DWORD nType = 0,nlevel = 0;
	TCHAR szBuffer[1024] = { 0 };
	In >> nType >> nlevel >> szBuffer;
	for (size_t i = 0; i < m_arLogHandle.size(); i++)
	{
		LogHandle* &pLogHandle = m_arLogHandle[i];
		if (pLogHandle != NULL)
			pLogHandle->Write(Id,(LogLevel)nlevel,szBuffer);
	}

}




END_NAMESPACE_KCORE