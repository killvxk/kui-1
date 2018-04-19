#include "StdAfx.h"
#include "channelsvr.h"
#include "CchannelMgr.h"

Cchannelsvr::Cchannelsvr(void)
{
}

Cchannelsvr::~Cchannelsvr(void)
{
}

Cchannelsvr& Cchannelsvr::GetInstance()
{
	static Cchannelsvr svr;
	return svr;
}

bool Cchannelsvr::StartServer()
{
	AfxWriteLogger(g_hChannelLogger, TEXT("initialize Cchannelsvr module ..."));

	if (!AfxCreateINetModule(m_pINetModule) || m_pINetModule == NULL)
		return false;

	m_pINetModule->SetNetCallback(this);

	if (m_pINetModule->Start(CHANNEL_TCP_PORT) != ERROR_SUCCESS)
		return false;

	return true;
}

void Cchannelsvr::StopServer()
{
	if (m_pINetModule != NULL)
	{
		m_pINetModule->Stop();
		m_pINetModule->Release();
		m_pINetModule = NULL;
	}

	AfxWriteLogger(g_hChannelLogger, TEXT("uninitialize Cchannelsvr module."));
}

void Cchannelsvr::Release()
{

}

long Cchannelsvr::OnAccept( INetModule* pNetModule, DWORD_PTR Id )
{
	return ERROR_SUCCESS;
}

long Cchannelsvr::OnSend( INetModule* pNetModule, DWORD_PTR Id )
{
	return ERROR_SUCCESS;
}	

long Cchannelsvr::OnReceive( INetModule* pNetModule, DWORD_PTR Id, LPCSTR pRecvBuf, LPSTR pSendBuf )
{
	CPkgHelper In((LPSTR)pRecvBuf);
	CPkgHelper Out(pSendBuf, PKG_MAXLENGTH, In.GetHeader()->command, In.GetHeader()->type);
	switch (In.GetHeader()->command)
	{
	case CMD_CLI_QUERYTASKLIST:		QueryTaskList(In, Out);			break;
	case CMD_CLI_DOWNTASKZIP:		DownTaskZip(In, Out);			break;
	case CMD_CLI_GETREPORTSTYLE:	QueryReportStyle(In,Out);		break;
	default: Out.PushErrorInfo(1, TEXT("unknow command."));			break;
	}
	return ERROR_SUCCESS;
}

void Cchannelsvr::OnClose( INetModule* pNetModule, DWORD_PTR Id )
{

}

void Cchannelsvr::QueryTaskList( CPkgHelper& In, CPkgHelper& Out )
{
	vector<TASKZIPINFO> arVec;
	sCchannelMgr.GetTaskZipInfo(arVec);
	Out << (DWORD)arVec.size();
	for (size_t i=0;i<arVec.size();i++)
		Out << arVec[i].nTaskId << arVec[i].nFileSize << arVec[i].nHashCode;
}

void Cchannelsvr::DownTaskZip(CPkgHelper& In, CPkgHelper& Out)
{
	sCchannelMgr.GetTaskZipFile(In,Out);
}

void Cchannelsvr::QueryReportStyle( CPkgHelper& In, CPkgHelper& Out )
{
	DWORD nStyle = sCchannelMgr.GetReportStyle();
	Out << nStyle;
}
