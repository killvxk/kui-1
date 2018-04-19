#pragma once
#include "public.h"
#include <functional>


START_NAMESPACE_KCORE



class Cklog_server : public INetCallback
{
public:
	Cklog_server();
	~Cklog_server();

public:
	static Cklog_server& GetInstance();
	bool StartKlogServer();
	void StopKlogServer();
public:
	virtual void Release()  { delete this; }
	virtual long OnAccept(INetModule* pNetModule, DWORD_PTR Id);
	virtual long OnSend(INetModule* pNetModule, DWORD_PTR Id);
	virtual long OnReceive(INetModule* pNetModule, DWORD_PTR Id, LPCSTR pRecvBuf, LPSTR pSendBuf);
	virtual void OnClose(INetModule* pNetModule, DWORD_PTR Id);

private:
	void HandleClientLog(DWORD_PTR Id,CPkgHelper& In);
private:
	INetModule*	m_pINetModule;
	vector<LogHandle*> m_arLogHandle;
};

#define sklogSvr		Cklog_server::GetInstance()

END_NAMESPACE_KCORE