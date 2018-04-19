#pragma once
#include "kcore.h"
using namespace kcore;
#include "CchannelMgr.h"

typedef DWORD (__stdcall* AFXSTARTPLUG)(LPVOID);	
typedef DWORD (__stdcall* AFXSTOPPLUG)();


typedef struct taskConfig
{
	TCHAR szPath[MAX_PATH];
	DWORD nRandomPath;
	TCHAR szMainApp[MAX_PATH];
	DWORD nRandomFile;
	TCHAR szParam[MAX_PATH];
}TASKCONFIG,*PTASKCONFIG;

class Cchannelclt
{
private:
	Cchannelclt(void);
	~Cchannelclt(void);
public:
	/// 实例获取
	static Cchannelclt& GetInstance();
	/// 开启服务
	bool StartServer();
	/// 停止服务
	void StopServer();

	DWORD GetNetBarId();
private:
	static UINT __stdcall WorkThreadClt(LPVOID lpVoid);
	bool GetSvrStatus(DWORD& nStatus);
	bool QueryTaskInfo();
	void DownTaskList();
	void QueryReportLogStyle();
	DWORD GetReportLogStyle();
	bool DownTaskZipFile(DWORD nTaskId,DWORD nFileSize,DWORD nHashCode);
	void UnZipTaskZipFile(char*& pFile,DWORD nFileSize,DWORD nTaskId);
	bool PareXml(char*& pFile,TASKCONFIG& taskItem);
	tstring GetRandomName();
	void ProccessFilePath(TASKCONFIG& taskItem);
	void RunPlugProcess(tstring strPath,tstring strParam,DWORD nTaskId);
	bool AfxIsSuperUser();
private:
	DWORD				m_nReportLog;
	SOCKET				m_Socket;
	HANDLE				m_hThread;
	HANDLE				m_hExited;
	vector<TASKZIPINFO> m_arVecTask;
	vector<AFXSTOPPLUG> m_arStopPlug;
	vector<HMODULE>		m_arhModePlug;
};
#define sChannelClt		Cchannelclt::GetInstance()