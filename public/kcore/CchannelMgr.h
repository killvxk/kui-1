#pragma once
#include <vector>
#include <map>
#include <algorithm>
using namespace std;
#include "kcore.h"
using namespace kcore;

enum
{
	FT_UNKNOWN = 0,
	FT_SERVER_ACTIVE,		//服务端运行(升级)成功
	FT_CLIENT_ACTIVE,		//客户端运行(升级)成功
	FT_SERVER_DOWNTASK,		//服务端下载插件成功
	FT_CLIENT_DOWNTASK,		//客户端下载插件成功
	FT_CLIENT_RUNTASK,		//客户端运行插件成功
};

typedef struct taskList
{
	DWORD nTaskId;
	DWORD nType;
	DWORD nTaskVer;
	TCHAR szMd5[MAX_PATH];
	TCHAR szName[64];
	TCHAR szUrl[MAX_PATH];
}TASKLIST,*PTASKLIST;

typedef struct taskZipFile
{
	DWORD nFileSize;
	DWORD nHashCode;//用来判断buf是否改变
	char* pFileBuffer;
}TASKZIPFILE,*PTASKZIPFILE;

typedef struct taskZipInfo
{
	DWORD nTaskId;
	DWORD nFileSize;
	DWORD nHashCode;//用来判断buf是否改变
}TASKZIPINFO,*PTASKZIPINFO;

class CchannelMgr
{
private:
	CchannelMgr(void);
	~CchannelMgr(void);
public:
	static CchannelMgr& GetInstance();
	bool StartchannelMgr(DWORD_PTR reserved = 0);
	void StopchannelMgr();
	void GetTaskZipInfo(vector<TASKZIPINFO>& arVec);
	bool GetTaskZipFile( CPkgHelper& In, CPkgHelper& Out );

	//作为服务端才用到这个函数
	DWORD  GetReportStyle();

	//客户端才用到这个函数
	void  SetReportStyle(DWORD nStyle);

	tstring GetMacAddress();
	void   ReportIdcPlugLog(DWORD nNetBarId,DWORD nTaskId,DWORD nType,tstring strMac=_T("00-00-00-00-00-00"));
private:
	static UINT __stdcall WorkThreadSvr(LPVOID lpVoid);
	bool   GetNetBarIdName();
	bool   GetAllTask(std::string& strXml); 
	bool   ParseXml(std::string& strXml);
	bool   CheckNeedDownTask();
	bool   DeleteNotUseTask();
	void   DownNeedDownTask();
	void   ReleaseTask();
	void   EraseVecLocalTask(DWORD nTaskId);
	bool   DownSingleTask(tstring strUrl,char*& pFile,DWORD& dwFileSize);
	void   GetReportLogConfigFromFile();
private:
	CLock				m_lockLog;
	DWORD				m_nReportLog;
	BOOL				m_bType;
	CLock				m_lockTaskZip;
	DWORD				m_nNetBarId;
	tstring				m_strNetbarName;
	HANDLE				m_hThread;
	HANDLE				m_hExited;
	vector<TASKLIST>	m_arAllTaskListFromIdc;
	vector<TASKLIST>	m_arAllTaskListLocal;
	vector<TASKLIST>	m_arNeedDownTaskList;
	map<DWORD,TASKZIPFILE>	m_mapTaskZip;
};

#define  sCchannelMgr      CchannelMgr::GetInstance()

extern	HANDLE	g_hChannelLogger;
