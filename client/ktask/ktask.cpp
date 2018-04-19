#include "stdafx.h"

#include <Iphlpapi.h>
#include <netioapi.h>
#include "khwmonitor.h"
#include <Psapi.h>
#include <tlhelp32.h>
#include <devguid.h>
#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include <thread>
#include <mutex>
#include "hy.h"
#include "kupdate.h"
#include "resource.h"
#include "imcpthread.hpp"
#include <setupapi.h>
#include <uuids.h>
#include <thread>
#include <Wbemidl.h>
#include <algorithm>
#include <devguid.h>
#include <Winbase.h>
#include "ssd_deployment_lib.h"
#include "kprocnotify.h"


#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "wbemuuid.lib")
#ifdef DEBUG
#pragma comment(lib,"kprocnotifylibd")
#else
#pragma comment(lib,"kprocnotifylib")
#endif // DEBUG


#pragma comment(lib, "Version.lib")

#import "msxml3.dll"
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Rpcrt4.lib")
//#pragma comment(lib, "../libssdopts/lib/kssdeploy.lib")

//ssd
typedef ssd_handle_t (__stdcall *CREATE_SSD)(const callback_t &, void *param);
typedef void (__stdcall *DESTROY_SSD)(ssd_handle_t handle);
typedef bool (__stdcall *START_SSD)(ssd_handle_t handle, const param_t &);
typedef void (__stdcall *STOP_SSD)(ssd_handle_t handle);

//hwmonitor
typedef bool (__stdcall *HWM_INIT)();
typedef void (__stdcall *HWM_UNINIT)();
typedef bool (__stdcall *GETTEMPERATURE)(tTemperature* pBuffer);
typedef bool (__stdcall *GETMAINBOARDNAMEINFO)(TCHAR* pBuffer,int nLenth);
typedef bool (__stdcall *GETRAMINFO)(TCHAR* pBuffer,int nLenth);
typedef bool (__stdcall *GETDISPLAYADAPTERINFO)(LPDISPLAYADAPTER lpDisplayAdapterInfo,int iNextIndex);

//rapidpowershutdown
typedef void(__stdcall *ENABLERAPIDPOWERSHUTDOWN)();
typedef void(__stdcall *DISABLERAPIDPOWERSHUTDOWN)();

#define IMAGE_DIR					TEXT("Skin\\Image\\")
#define SOFTWARE_KEY				TEXT("SOFTWARE\\{3798BE84-4E13-4b81-B8CF-5063730FF905}")
#define SOFTKEY_GINFO				SOFTWARE_KEY TEXT("\\GINFO")
#define SOFTKEY_VDINFO				SOFTWARE_KEY TEXT("\\VDINFO")
#define SOFTKEY_GEFFECT				SOFTWARE_KEY TEXT("\\GEFFECT")
#define VNC_SERVIECE_NAME			TEXT("kvnserver.exe")
#define IEHIST_PROCESS_NAME			TEXT("kieclient.exe")
#define KDESK_PROCESS_NAME			TEXT("kdesk.exe")
#define KHARDWARE_PROCESS_NAME		TEXT("khardware.exe")


void WritedeskProtocol()
{
	TCHAR szCurrentUser[MAX_PATH] = {0};
	AfxGetCurUserHKeyRoot(szCurrentUser, sizeof(szCurrentUser));
	lstrcat(szCurrentUser,_T("\\Software\\Microsoft\\Internet Explorer\\ProtocolExecute\\cgm"));
	DWORD dwValue = 0;
	SHSetValue(HKEY_USERS, szCurrentUser, TEXT("WarnOnOpen"), REG_DWORD, &dwValue, sizeof(DWORD));

#define DESK_PROT	TEXT("URL:cgm Protocol")
#define DESK_VER	TEXT("0.1")
#define DESK_ICON	TEXT("")

	SHSetValue(HKEY_CLASSES_ROOT, TEXT("cgm"), TEXT(""), REG_SZ, DESK_PROT, (DWORD)(_tcsclen(DESK_PROT)* sizeof(TCHAR)));
	SHSetValue(HKEY_CLASSES_ROOT, TEXT("cgm"), TEXT("@"), REG_SZ, DESK_PROT, (DWORD)(_tcsclen(DESK_PROT)* sizeof(TCHAR)));
	SHSetValue(HKEY_CLASSES_ROOT, TEXT("cgm"), TEXT("Version"), REG_SZ, DESK_VER, (DWORD)(_tcsclen(DESK_VER) * sizeof(TCHAR)));
	SHSetValue(HKEY_CLASSES_ROOT, TEXT("cgm"), TEXT("URL Protocol"), REG_SZ, TEXT(""), 0);

	TCHAR szBufExe[MAX_PATH] = {0};
	tstring strExe = AfxGetAppPath(szBufExe,MAX_PATH,_T("knbmenu.exe"));
	SHSetValue(HKEY_CLASSES_ROOT, TEXT("cgm\\DefaultIcon"), TEXT(""), REG_SZ, strExe.c_str(), (DWORD)(strExe.size() * sizeof(TCHAR)));
	strExe += TEXT(" \"%1\"");
	SHSetValue(HKEY_CLASSES_ROOT, TEXT("cgm\\Shell\\Open\\Command"), TEXT(""), REG_SZ, strExe.c_str(), (DWORD)(strExe.size() * sizeof(TCHAR)));
}
static bool CompareSvrList(tagServerList svr1, tagServerList svr2)
{
	return svr1.Id < svr2.Id;
}
static bool CompareSvrIpList(DWORD nIp1, DWORD nIp2)
{
	return nIp1 < nIp2;
}
static bool CompareIpList(tstring strip1, tstring strip2)
{
	return lstrcmpi(strip1.c_str(), strip2.c_str()) < 0;
}
SOCKET gSocket  = INVALID_SOCKET;


typedef struct tBootTask
{
	TCHAR TaskId[MAX_PATH];
	DWORD TaskType;
	TCHAR SvrDir[MAX_PATH];
	TCHAR CliDir[MAX_PATH];
	TCHAR ExeFile[MAX_PATH];
	TCHAR Param[MAX_PATH];
	TCHAR MirrorStr[MAX_PATH];
	DWORD HyExecute;
	DWORD DelayTime;
	DWORD systemrun;
	DWORD TimeTicket;	// 计算相对时间临时使用
	DWORD lastExTime;
}tBootTask;

bool sort_by_DelayTime(const  tBootTask &left, const  tBootTask&right)
{
	return left.DelayTime < right.DelayTime;
}

void formatfilepath(wchar_t* path)
{
	wchar_t * p = path;
	while (*p != L'\0')
	{		
		if (*p == L'\\')
			*p = L'/';
			p++;
	}
}

class CTaskServer
{
public:
	CTaskServer() : ghMainThread(NULL), ghUdpThread(NULL), ghVidskThread(NULL), ghTaskThread(NULL), 
		ghNotifyMenuEvent(NULL), ghPushThread(NULL), gIp(0),m_hFileMap(NULL),gGameServerIp(0),/*ghReportTempThread(NULL),*/
		m_bReconnect(FALSE),m_bPushWallpaper(FALSE),m_pSsdHandle(NULL),m_hSsdLogger(NULL),m_bDeploy(FALSE)
	{
		CoInitialize(NULL);
		m_pCreate_Ssd = NULL;
		m_pDestroy_Ssd = NULL;
		m_pStart_Ssd = NULL;
		m_pStop_Ssd = NULL;

		m_bInitHw = false;
		m_pHwmonitor_Init = NULL;
		m_pHwmonitor_UnInit = NULL;
		m_pGetTemperature = NULL;
		m_pGetMainBoardNameInfo = NULL;
		m_pGetRamInfo = NULL;
		m_pGetDisply = NULL;

		m_pEnableRapidPowerShutDown = NULL;
		m_pDisableRapidPowerShutDown = NULL;

		ZeroMemory(gServer, sizeof(gServer));
		ZeroMemory(gGameServerName,sizeof(gGameServerName));
		ZeroMemory(gMachine, sizeof(gMachine));
		ZeroMemory(gNetCard, sizeof(gNetCard));
		ZeroMemory(gMac, sizeof(gMac));
		DWORD nSize = sizeof(gMachine) - 1;
		GetComputerName(gMachine, &nSize);
		_tcsupr_s(gMachine);
		AfxSetPrivilege(SE_DEBUG_NAME, TRUE);
		dorunmenu = NULL;
	}
	~CTaskServer() 
	{
		if (m_hFileMap != NULL)
		{
			CloseHandle(m_hFileMap);
			m_hFileMap = NULL;
		}
		if(m_hSsdLogger)
		{
			AfxWriteLogger(m_hSsdLogger, TEXT("=============================================="));
			AfxCloseLogger(m_hSsdLogger);
		}
		WSACleanup();
		CoUninitialize();
	}
	static CTaskServer* getInstance()
	{
		static CTaskServer gTaskServer;
		return &gTaskServer;
	}
public:
	bool StartTaskServer();
	void StopTaskServer();
private:
	bool ExactIconFromDb(DWORD gid, LPCTSTR szIconFile);
	SOCKET CreateUdpSocket();
	tstring GetSysOptString(LPCTSTR szKey, LPCTSTR szDefValue = TEXT(""));
	long GetSysOptInteger(LPCTSTR szKey, long nDefValue = 0);
	bool DownLoadFile(LPCTSTR szSrcFile, LPCTSTR szDstFile, DWORD dwFileSize = -1, DWORD dwFileModifyTime = -1, BOOL bIsRelative = TRUE);

	//inject
	void LoadRapidPowerShutdown();
	void UnloadRapidPowerShutdown();
	//report client network and hardware info.
	bool ReportBaseInfo();
	void ReportHardwareInfo();
	void ReportTemprature();

	//get off line data.
	bool WriteDataFile(LPCTSTR szFileName, LPVOID pData, int length);
	bool ReadDataFile(LPCTSTR szFileName, LPSTR& pData, DWORD& dwLength);
	void GetSystemOption();
	void GetServerList();
	void GetVdiskList();
	void GetServerListEx();
	void GetVdiskListEx();
	void WriteGamesInfo(std::map<DWORD, tGameInfo>& GameInfo);
	void WriteGameInfoRegist();
	void GetGameInfo();
	void GetGameConfig();
	void GetRateInfo();

	void GetIconInfo();
	int	 DownIconCount;
	void MutiThreadGetIconInfo();
	void DownGetIconInfo();
	std::mutex mtx;
	std::set<DWORD> gidss;
	std::vector<std::thread*> threadpool;



	bool GetNeedUpdateIconList(std::set<DWORD>& gids);
	bool GetIconInfoByGid(CPkgHelper& pkgIn);
	bool GetIconInfoByGid(SOCKET  so,CPkgHelper& pkgIn);

	void WriteIconData();
	void GetSsdWorkMode();

	void ParerIdcPushXml();
	void CreateIdcPush(std::vector<tagShortCutIconInfo>& arShortCuts,std::vector<tagWallPaper>& arWallPaper);

	//execute exe and other function.
	void GetDeskTopLinkPath(LPTSTR szDesktop);
	bool CreateDesktopLink(LPCTSTR name, LPCTSTR exe, LPCTSTR param, LPCTSTR icon, LPCTSTR memo, bool &bRefresh);
	bool DeleteMoreLink(std::map<tstring, tstring>& LinkList);
	void CreateLinkHelper();
	void SetWallPaper();
	void RunExeHelper();
	void RunNetMoonClient();
	BOOL FixPubwin();
	//execute boot task.
	bool ExecuteSingleBootTask(std::vector<tBootTask>::iterator& it);
	//void ExecuteSingleBootTask(tBootTask& Task);
	//broad to get main server ip
	DWORD  GetServerIpUseBroad(bool bTakeOver);
	//process udp notify
	tstring TranslateImagePath(const tstring& source);
	void ViewProcessList();
	void BaseInfoQuery();
	bool GetExpertMemory(HANDLE proid,size_t& value);	
	void BaseDriverInfo();
	void UdpRecvNotify(LPCSTR pData, DWORD length, sockaddr_in& from);

	void PushSingleGame(HANDLE hNotify, tGameInfo& GameInfo, DWORD speed);

	BOOL LoadSafeDll(LPCTSTR ip, int port);

private:
	SIZE_T pagesize;
	static UINT __stdcall MainWorkThread(LPVOID lpVoid);
	static UINT __stdcall VdiskWorkThread(LPVOID lpVoid);
	static UINT __stdcall VdiskWorkThreadEx(LPVOID lpVoid);
	static UINT __stdcall DispathTaskThread(LPVOID lpVoid);
	static UINT __stdcall UdpWorkThread(LPVOID lpVoid);
	static UINT __stdcall PushGameThread(LPVOID lpVoid);
	static UINT __stdcall ReportTempratureThread(LPVOID lpVoid);
	void WriteShareMemory(LPCTSTR szAgentId,LPCTSTR szNetId);
	bool Heart();
	void Reconnect();
	bool DownGameDesktopIco(DWORD gid,LPCTSTR szIconFile);
	void SaveDialogInfo();
	void LoadDialogInfo();
	BOOL IsFromMainSvr(DWORD nIp);
	bool GetSvrStatus(DWORD& nStatus);
	void DownLoadMenuBg();
	void DownLoadResZip();
	void InitHwmonitor();
	void UnitHwmonitor();
	public:
	DWORD headtime;
	static bool needmenu;
	int  Runknbmenu(bool needmenu);
	void DoRegBeforMenue();
	void DorunMenu();
	std::thread *dorunmenu;
	std::vector<tBootTask> BootTask;
	
private:
	CREATE_SSD		m_pCreate_Ssd;
	DESTROY_SSD		m_pDestroy_Ssd;
	START_SSD       m_pStart_Ssd;
	STOP_SSD		m_pStop_Ssd;



	
	//hwmonitor
	bool					m_bInitHw;
	HWM_INIT				m_pHwmonitor_Init;
	HWM_UNINIT				m_pHwmonitor_UnInit;
	GETTEMPERATURE			m_pGetTemperature;
	GETMAINBOARDNAMEINFO	m_pGetMainBoardNameInfo;
	GETRAMINFO				m_pGetRamInfo;
	GETDISPLAYADAPTERINFO	m_pGetDisply;
	//rapidpowershutdown
	ENABLERAPIDPOWERSHUTDOWN	m_pEnableRapidPowerShutDown;
	DISABLERAPIDPOWERSHUTDOWN	m_pDisableRapidPowerShutDown;
	bool   m_bDeploy;
	HANDLE m_hSsdLogger;
	void*  m_pSsdHandle;
	BOOL   m_bPushWallpaper;
	BOOL   m_bReconnect;
	HANDLE ghMainThread;
	HANDLE ghUdpThread;
	HANDLE ghVidskThread;
	HANDLE ghTaskThread;
	HANDLE ghPushThread;
	//HANDLE ghReportTempThread;
	HANDLE ghNotifyMenuEvent;
	DWORD gIp;
	TCHAR gServer[MAX_PATH];
	TCHAR gGameServerName[MAX_PATH];
	DWORD gGameServerIp;
	TCHAR gMachine[MAX_COMPUTERNAME_LENGTH + 1];
	TCHAR gNetCard[MAX_PATH];
	std::vector<tGameConfig>   gGameConfig;
	std::map<DWORD, tGameInfo> gGameInfo;
	std::map<DWORD, tGameInfo> gPushInfo;
	std::map<DWORD, tagIconInfo> gIconInfo;
	std::map<tstring, tstring> gSysOption;
	std::vector<tDiskMgr>  gDiskMgr; // 这里都是游戏盘
	std::map<tstring, tstring> gSvrList;
	std::vector<tagServerList>			   gSvrListEx;
	std::vector<tagAllServerGameDisk>	   gDiskMgrEx;
	std::map<tstring, tstring> m_LinkList;//保存创建的快捷方式
	//网吧桌面工具
	std::vector<DIALOG_INFO>			gDialog;
	std::vector<ItemShortCut>			gItemShortCut;
	//共享内存
	HANDLE m_hFileMap;
	TCHAR  gMac[MAX_MAC_SIZE];

	//imcp thread
	imcpthread imcp;
};

bool CTaskServer::StartTaskServer()
{
	headtime = GetTickCount();
	CEvenyOneSD sd;
	ghNotifyMenuEvent = CreateEvent(sd.GetSA(),FALSE,FALSE,_T("Global\\_pkg_refresh_data"));
	if (ghNotifyMenuEvent == NULL)
		return false;

	ghMainThread = (HANDLE)_beginthreadex(NULL, 0, MainWorkThread, this, 0, NULL);
	if (ghMainThread == NULL)
		return false;
	ghUdpThread = (HANDLE)_beginthreadex(NULL, 0, UdpWorkThread, this, 0, NULL);
	if (ghUdpThread == NULL)
	{
		SetEvent(ghExited);
		return false;
	}

	WritedeskProtocol();

	BeginWork(ghExited);

	return true;
}

void CTaskServer::StopTaskServer()
{	
	EndWork();

	if (dorunmenu)
	{
		dorunmenu->join();
		delete dorunmenu;
		dorunmenu = NULL;
	}
	HANDLE hEvents[] = {ghTaskThread, ghVidskThread, ghUdpThread, ghMainThread, ghPushThread/*,ghReportTempThread*/};
	for (int idx=0; idx<_countof(hEvents); idx++)
	{
		HANDLE hEvent = hEvents[idx];
		if (hEvent != NULL)
		{
			WaitForSingleObject(hEvent, INFINITE);
			CloseHandle(hEvent);
		}
	}
	ghMainThread = ghUdpThread = ghVidskThread = ghTaskThread = /*ghReportTempThread =*/ NULL;

	if (ghNotifyMenuEvent != NULL)
		CloseHandle(ghNotifyMenuEvent);
	ghNotifyMenuEvent = NULL;

	if(m_pSsdHandle)
		m_pStop_Ssd(m_pSsdHandle);

	if(m_pSsdHandle)
	{
		m_pDestroy_Ssd(m_pSsdHandle);
		m_pSsdHandle = NULL;
	}

	AfxKillProcess(IEHIST_PROCESS_NAME);
	AfxKillProcess(KDESK_PROCESS_NAME);
	AfxKillProcess(KHARDWARE_PROCESS_NAME);
}
bool CTaskServer::Heart()
{
	int length = PKG_MAXLENGTH;
	char szSend[PKG_MAXLENGTH] = {0}, szRecv[PKG_MAXLENGTH] = {0};
	kcore::CPkgHelper pkgSend(szSend, CMD_CLI_HEART, OBJECT_CLIENT);
	if ( ERROR_SUCCESS == AfxSocketExecuteLock(gSocket, szSend, pkgSend.GetLength(), szRecv, length))
	{
		kcore::CPkgHelper pkgRecv(szRecv);
		if (pkgRecv.IsStatusOk())
		{
			return true;
		}
	}
	return false;
}
void CTaskServer::Reconnect()
{
	m_bReconnect = TRUE;
	if(gSocket != INVALID_SOCKET)
		AfxSocketClose(gSocket);

	TCHAR szServerIp[MAX_PATH] = {0};
	AfxGetSysString(TEXT("config"), TEXT("serverip"), szServerIp, _countof(szServerIp), TEXT("127.0.0.1"));
	gSocket = AfxSocketConnect(szServerIp, APP_TCP_PORT);

	if(gSocket != INVALID_SOCKET)
		ReportBaseInfo();
}
bool CTaskServer::DownGameDesktopIco(DWORD gid,LPCTSTR szIconFile)
{
	bool bResult = true;
	try
	{
		TCHAR szIconSrc[MAX_PATH] = {0};
		if(gid > 0)
		{
			char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
			CPkgHelper pkgSend(szSendBuffer, CMD_CLI_QUERYGAMEICOPATH, OBJECT_CLIENT);
			pkgSend << gid;
			if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, szSendBuffer, pkgSend.GetLength(), szRecvBuffer, _countof(szRecvBuffer)))
				throw (int)'e';

			CPkgHelper pkgRecv(szRecvBuffer);
			if (!pkgRecv.IsStatusOk())
				throw (int)'w';
			pkgRecv >> szIconSrc;
		}

		if(_tcslen(szIconSrc) > 0)
			if(!DownLoadFile(szIconSrc,szIconFile,-1,-1,false))
				throw (int)'l';

	}
	catch (int &e) {
		AfxWriteLogger(ghLogger, _T("DownGameDesktopIco failed! %d"), e);
		bResult = false;
	}
	catch (...) { 
		AfxWriteLogger(ghLogger, _T("down [%d] desktop shortcut ico failed!"), gid);
		bResult = false; 
	}

	return bResult;
}
bool CTaskServer::ExactIconFromDb(DWORD gid, LPCTSTR szIconFile)
{
	bool bret = false;
	/*sqlite3* pSqlite = NULL;
	TCHAR szDbFile[MAX_PATH] = {0}, szSql[MAX_PATH] = {0};
	try
	{
	AfxGetAppPath(szDbFile, MAX_PATH, TEXT("config\\kicon.dat"));
	if (SQLITE_OK != sqlite3_open(_bstr_t(szDbFile), &pSqlite))
	throw 0;
	sqlite3_stmt *pstat = NULL;	
	_stprintf_s(szSql, _countof(szSql), TEXT("SELECT GID, DATA FROM tIconList WHERE GID = %d"), gid);
	int result = sqlite3_prepare(pSqlite, _bstr_t(szSql), -1, &pstat, NULL);
	if (result != SQLITE_OK || sqlite3_step(pstat) != SQLITE_ROW)
	throw 0;

	int size = sqlite3_column_bytes(pstat, 1);
	char* pdata = (char*)sqlite3_column_blob(pstat, 1);
	if (size && pdata)
	{
	SetFileAttributes(szIconFile, FILE_ATTRIBUTE_NORMAL);
	AfxCreateDirectory(szIconFile);
	HANDLE hFile = CreateFile(szIconFile, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	throw 0;

	DWORD dwWriteBytes = 0; 
	WriteFile(hFile, pdata, size, &dwWriteBytes, NULL);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	}
	sqlite3_finalize(pstat);
	}
	catch (...) { bret = false; };
	if (pSqlite)
	sqlite3_close(pSqlite);*/
	std::map<DWORD,tagIconInfo>::iterator iter = gIconInfo.find(gid);
	if (iter != gIconInfo.end() && iter->second.dwLength > 0 && iter->second.pIcon != NULL)
	{
		SetFileAttributes(szIconFile, FILE_ATTRIBUTE_NORMAL);
		AfxCreateDirectory(szIconFile);
		HANDLE hFile = CreateFile(szIconFile, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return bret;

		DWORD dwWriteBytes = 0; 
		if(WriteFile(hFile, iter->second.pIcon, iter->second.dwLength, &dwWriteBytes, NULL) &&
			dwWriteBytes == iter->second.dwLength)
			bret = true;
		SetEndOfFile(hFile);
		CloseHandle(hFile);
	}
	return bret;
}

SOCKET CTaskServer::CreateUdpSocket()
{
	SOCKET sck = socket(AF_INET, SOCK_DGRAM, 0);
	if (sck == INVALID_SOCKET)
	{
		AfxWriteLogger(ghLogger, TEXT("create udp socket erorr:%d."), WSAGetLastError());
		return INVALID_SOCKET;
	}
	sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(CLI_UDP_PORT);
	addr.sin_addr.s_addr = 0;
	if (SOCKET_ERROR == bind(sck, (PSOCKADDR)&addr, sizeof(addr)))
	{
		AfxWriteLogger(ghLogger, TEXT("bind udp socket erorr:%d."), WSAGetLastError());
		closesocket(sck);
		return INVALID_SOCKET;
	}

	int buf_size = PKG_MAXLENGTH;
	setsockopt(sck, SOL_SOCKET, SO_RCVBUF, (char *)&buf_size, sizeof(buf_size));
	linger InternalLinger = { 1, 0 };
	setsockopt(sck, SOL_SOCKET, SO_LINGER, (const char*)&InternalLinger, sizeof(linger));
	return sck;
}

tstring CTaskServer::GetSysOptString(LPCTSTR szKey, LPCTSTR szDefValue)
{
	std::map<tstring, tstring>::iterator iter = gSysOption.find(szKey);
	if (iter != gSysOption.end())
	{
		if (iter->second.size())
			return iter->second;
	}
	return tstring(szDefValue);
}

long CTaskServer::GetSysOptInteger(LPCTSTR szKey, long nDefValue)
{
	std::map<tstring, tstring>::iterator iter = gSysOption.find(szKey);
	if (iter != gSysOption.end())
	{
		if (iter->second.size())
			return _ttoi(iter->second.c_str());
	}
	return nDefValue;
}

bool CTaskServer::DownLoadFile(LPCTSTR szSrcFile, LPCTSTR szDstFile, DWORD dwFileSize /* = -1 */, DWORD dwFileModifyTime /* = -1 */, BOOL bIsRelative /* = TRUE */)
{
	static const DWORD FILE_BLOCK_SIZE = 0x8000;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	bool bRet = false;
	try
	{
		if (dwFileSize == -1)
		{
			char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
			CPkgHelper pkgSend(szSendBuffer, CMD_CLI_QUERYFILEINFO, OBJECT_CLIENT);
			pkgSend << bIsRelative << szSrcFile;
			if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, szSendBuffer, pkgSend.GetLength(), szRecvBuffer, _countof(szRecvBuffer)))
				throw (int)'e';

			CPkgHelper pkgRecv(szRecvBuffer);
			if (!pkgRecv.IsStatusOk())
				throw (int)'w';
			pkgRecv >> dwFileSize >> dwFileModifyTime;
		}

		SetFileAttributes(szDstFile, FILE_ATTRIBUTE_NORMAL);
		AfxCreateDirectory(szDstFile);
		if (PathIsDirectory(szDstFile))
			return true;
		hFile = ::CreateFile(szDstFile, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			throw (int)'i';
		DWORD left = dwFileSize;
		while (left)
		{
			char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
			CPkgHelper pkgSend(szSendBuffer, CMD_CLI_DOWNFILEBLOCK, OBJECT_CLIENT);
			DWORD dwBlkSize = left >= FILE_BLOCK_SIZE ? FILE_BLOCK_SIZE : left;
			pkgSend << bIsRelative << szSrcFile << dwFileSize - left << dwBlkSize;
			if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, szSendBuffer, pkgSend.GetLength(), szRecvBuffer, sizeof(szRecvBuffer)))
				throw (int)'ee';
			CPkgHelper pkgRecv(szRecvBuffer);
			if (!pkgRecv.IsStatusOk())
				throw (int)'ww';
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				break;
			DWORD dwByteWrite = 0;
			if (!WriteFile(hFile, szRecvBuffer + sizeof(PkgHeader), dwBlkSize, &dwByteWrite, NULL) || dwBlkSize != dwByteWrite)
				throw (int)'wf';
			left -= dwBlkSize;
		}
		SetEndOfFile(hFile);
		if (dwFileModifyTime != -1)
		{
			FILETIME ft = {0};
			AfxUtcTime2FileTime(dwFileModifyTime, ft);
			SetFileTime(hFile, &ft, &ft, &ft);
		}
		bRet = true;
	}
	catch (int &e) {
		AfxWriteLogger(ghLogger, _T("DownLoadFile  failed! %d"), e);
		bRet = false;
	}
	catch (...) { 
		AfxWriteLogger(ghLogger, _T("down [%s] to [%s] failed!"), szSrcFile, szDstFile);
		bRet = false; 
	}
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if (!bRet)
		DeleteFile(szDstFile);
	return bRet;
}

typedef enum _NDIS_MEDIUM
{
	NdisMedium802_3,
	NdisMedium802_5,
	NdisMediumFddi,
	NdisMediumWan,
	NdisMediumLocalTalk,
	NdisMediumDix,              // defined for convenience, not a real medium
	NdisMediumArcnetRaw,
	NdisMediumArcnet878_2,
	NdisMediumAtm,
	NdisMediumWirelessWan,
	NdisMediumIrda,
	NdisMediumBpc,
	NdisMediumCoWan,
	NdisMedium1394,
	NdisMediumInfiniBand,
#if ((NTDDI_VERSION >= NTDDI_LONGHORN) || NDIS_SUPPORT_NDIS6)
	NdisMediumTunnel,
	NdisMediumNative802_11,
	NdisMediumLoopback,
#endif // (NTDDI_VERSION >= NTDDI_LONGHORN)
	NdisMediumMax               // Not a real medium, defined as an upper-bound
} NDIS_MEDIUM, *PNDIS_MEDIUM;

typedef enum _NDIS_PHYSICAL_MEDIUM
{
	NdisPhysicalMediumUnspecified,
	NdisPhysicalMediumWirelessLan,
	NdisPhysicalMediumCableModem,
	NdisPhysicalMediumPhoneLine,
	NdisPhysicalMediumPowerLine,
	NdisPhysicalMediumDSL,      // includes ADSL and UADSL (G.Lite)
	NdisPhysicalMediumFibreChannel,
	NdisPhysicalMedium1394,
	NdisPhysicalMediumWirelessWan,
	NdisPhysicalMediumNative802_11,
	NdisPhysicalMediumBluetooth,
	NdisPhysicalMediumInfiniband,
	NdisPhysicalMediumWiMax,
	NdisPhysicalMediumUWB,
	NdisPhysicalMedium802_3,
	NdisPhysicalMedium802_5,
	NdisPhysicalMediumIrda,
	NdisPhysicalMediumWiredWAN,
	NdisPhysicalMediumWiredCoWan,
	NdisPhysicalMediumOther,
	NdisPhysicalMediumMax       // Not a real physical type, defined as an upper-bound
} NDIS_PHYSICAL_MEDIUM, *PNDIS_PHYSICAL_MEDIUM;

typedef struct _MIB_IF_ROW2 {
	//
	// Key structure.  Sorted by preference.
	//
	NET_LUID InterfaceLuid;
	NET_IFINDEX InterfaceIndex; 

	//
	// Read-Only fields.
	//
	GUID InterfaceGuid;
	WCHAR Alias[IF_MAX_STRING_SIZE + 1]; 
	WCHAR Description[IF_MAX_STRING_SIZE + 1];
	ULONG PhysicalAddressLength;
	UCHAR PhysicalAddress[IF_MAX_PHYS_ADDRESS_LENGTH];
	UCHAR PermanentPhysicalAddress[IF_MAX_PHYS_ADDRESS_LENGTH];    

	ULONG Mtu;
	IFTYPE Type;                // Interface Type.
	TUNNEL_TYPE TunnelType;     // Tunnel Type, if Type = IF_TUNNEL.
	NDIS_MEDIUM MediaType; 
	NDIS_PHYSICAL_MEDIUM PhysicalMediumType; 
	NET_IF_ACCESS_TYPE AccessType;
	NET_IF_DIRECTION_TYPE DirectionType;
	struct {
		BOOLEAN HardwareInterface : 1;
		BOOLEAN FilterInterface : 1;
		BOOLEAN ConnectorPresent : 1;
		BOOLEAN NotAuthenticated : 1;
		BOOLEAN NotMediaConnected : 1;
		BOOLEAN Paused : 1;
		BOOLEAN LowPower : 1;
		BOOLEAN EndPointInterface : 1;
	} InterfaceAndOperStatusFlags;

	IF_OPER_STATUS OperStatus;  
	NET_IF_ADMIN_STATUS AdminStatus;
	NET_IF_MEDIA_CONNECT_STATE MediaConnectState;
	NET_IF_NETWORK_GUID NetworkGuid;
	NET_IF_CONNECTION_TYPE ConnectionType; 

	//
	// Statistics.
	//
	ULONG64 TransmitLinkSpeed;
	ULONG64 ReceiveLinkSpeed;

	ULONG64 InOctets;
	ULONG64 InUcastPkts;
	ULONG64 InNUcastPkts;
	ULONG64 InDiscards;
	ULONG64 InErrors;
	ULONG64 InUnknownProtos;
	ULONG64 InUcastOctets;      
	ULONG64 InMulticastOctets;  
	ULONG64 InBroadcastOctets; 
	ULONG64 OutOctets;
	ULONG64 OutUcastPkts;
	ULONG64 OutNUcastPkts;
	ULONG64 OutDiscards;
	ULONG64 OutErrors;
	ULONG64 OutUcastOctets;     
	ULONG64 OutMulticastOctets; 
	ULONG64 OutBroadcastOctets;   
	ULONG64 OutQLen; 
} MIB_IF_ROW2, *PMIB_IF_ROW2;

typedef DWORD (WINAPI* GetIfEntry2_Func)(PMIB_IF_ROW2 Row);
bool CTaskServer::needmenu = false;

bool CTaskServer::ReportBaseInfo()
{
	bool bResult = true;
	tClientInfo Client = {0};

	//get base info.
	{
		AfxGetFileVersion(TEXT("knbclient.exe"), Client.Version);
		for (TCHAR chr=TEXT('C'); chr <= TEXT('Z'); chr++)
		{
			TCHAR szDriver[] = TEXT("X:\\");
			szDriver[0] = chr;
			if (GetDriveType(szDriver) == DRIVE_FIXED)
			{
				szDriver[1] = 0;
				lstrcat(Client.AllArea, szDriver);
				if (hy_DriverProtected(szDriver[0]))
					lstrcat(Client.ProtArea, szDriver);
			}
		}

		char* pIp = NULL;
		DWORD dwIp = 0;
		sockaddr_in addr = {0};
		int nSize = sizeof(addr);
		if (getsockname(gSocket, (PSOCKADDR)&addr, &nSize) != SOCKET_ERROR && (pIp = inet_ntoa(addr.sin_addr)) != NULL)
		{
			dwIp = inet_addr(pIp);
			if ((dwIp & 0xff) == 127)
				dwIp = 0;
		}

		DWORD dwSize = 0;
		PIP_ADAPTER_INFO pAdapterInfo = NULL;
		if (GetAdaptersInfo(pAdapterInfo, &dwSize) != ERROR_BUFFER_OVERFLOW)
			return false;
		pAdapterInfo = (PIP_ADAPTER_INFO)(new char[dwSize]);
		if (GetAdaptersInfo(pAdapterInfo, &dwSize) != NOERROR)
		{
			delete [] (char*)pAdapterInfo;
			return false;
		}
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo, pNextAdapter = pAdapterInfo;
		while(pNextAdapter)
		{
			pAdapter = pNextAdapter;
			pNextAdapter = pAdapter->Next;

			DWORD dwVersion = GetVersion();

			DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
			DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

			if (dwMajorVersion >= 6)
			{
				// Vista 以上 // 可以读取万兆网卡的速度
				HMODULE hDll = GetModuleHandle(_T("Iphlpapi.dll"));
				GetIfEntry2_Func func = NULL;
				if (hDll)
				{
					func = (GetIfEntry2_Func)GetProcAddress(hDll, "GetIfEntry2");
				}

				if (func == NULL)
				{
					continue;
				}

				MIB_IF_ROW2 IfRow = {0};
				IfRow.InterfaceIndex = pAdapter->Index;
				if (func(&IfRow) != NO_ERROR || inet_addr(pAdapter->IpAddressList.IpAddress.String) == 0 || 
					inet_addr(pAdapter->GatewayList.IpAddress.String) == 0 ||
					IfRow.OperStatus != IfOperStatusUp)
					continue;

				if (dwIp != 0 && inet_addr(pAdapter->IpAddressList.IpAddress.String) != dwIp)
					continue;

				pNextAdapter = NULL;		//for exit.
				lstrcpy(gNetCard, _bstr_t(pAdapter->Description));
				Client.Ip   = inet_addr(pAdapter->IpAddressList.IpAddress.String);
				gIp = Client.Ip;
				AfxSetSysString(TEXT("config"), TEXT("localip"), _bstr_t(pAdapter->IpAddressList.IpAddress.String));
				Client.Gate = inet_addr(pAdapter->GatewayList.IpAddress.String);
				Client.Mark = inet_addr(pAdapter->IpAddressList.IpMask.String);
				Client.NetSpeed = (DWORD)(IfRow.TransmitLinkSpeed / (1000 * 1000));
			}
			else
			{
				MIB_IFROW IfRow = {0};
				IfRow.dwIndex = pAdapter->Index;
				if (GetIfEntry(&IfRow) != NO_ERROR || inet_addr(pAdapter->IpAddressList.IpAddress.String) == 0 || 
					inet_addr(pAdapter->GatewayList.IpAddress.String) == 0 ||
					IfRow.dwOperStatus != IF_OPER_STATUS_OPERATIONAL)
					continue;

				if (dwIp != 0 && inet_addr(pAdapter->IpAddressList.IpAddress.String) != dwIp)
					continue;

				pNextAdapter = NULL;		//for exit.
				lstrcpy(gNetCard, _bstr_t(pAdapter->Description));
				Client.Ip   = inet_addr(pAdapter->IpAddressList.IpAddress.String);
				gIp = Client.Ip;
				AfxSetSysString(TEXT("config"), TEXT("localip"), _bstr_t(pAdapter->IpAddressList.IpAddress.String));
				Client.Gate = inet_addr(pAdapter->GatewayList.IpAddress.String);
				Client.Mark = inet_addr(pAdapter->IpAddressList.IpMask.String);
				Client.NetSpeed = IfRow.dwSpeed / (1000 * 1000);
			}

			_stprintf_s(Client.Mac, _countof(Client.Mac), TEXT("%02X-%02X-%02X-%02X-%02X-%02X"),
				pAdapter->Address[0],
				pAdapter->Address[1],
				pAdapter->Address[2],
				pAdapter->Address[3],
				pAdapter->Address[4],
				pAdapter->Address[5]);

			lstrcpy(gMac,Client.Mac);
			PIP_PER_ADAPTER_INFO pPerAdapter = NULL;
			DWORD dwSize = 0;
			if (GetPerAdapterInfo(pAdapter->Index, pPerAdapter, &dwSize) == ERROR_BUFFER_OVERFLOW)
			{
				pPerAdapter = PIP_PER_ADAPTER_INFO(new char[dwSize]);
				if (GetPerAdapterInfo(pAdapter->Index, pPerAdapter, &dwSize) == NO_ERROR)
				{
					Client.DNS = inet_addr(pPerAdapter->DnsServerList.IpAddress.String);
					if(pPerAdapter->DnsServerList.Next != NULL)
						Client.DNS2 = inet_addr(pPerAdapter->DnsServerList.Next->IpAddress.String);
				}
				delete[] (char*)(pPerAdapter);
			}
		}
		delete [] (char*)pAdapterInfo;
	}

	//arp install status
	HANDLE  h_sys = CreateFile(TEXT("\\\\.\\KArp"),0,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
	Client.ArpRunStatus = h_sys != INVALID_HANDLE_VALUE ? 1:0;
	if(h_sys != INVALID_HANDLE_VALUE)
	{
		CloseHandle(h_sys);
		h_sys = INVALID_HANDLE_VALUE;
	}

	DWORD mode_flag;
	DISKLESS_INFO diskless_info;
	memset(&diskless_info, 0, sizeof(diskless_info));
	ScsiDiskQueryDisklessInfoEx(&diskless_info);
	if(diskless_info.diskless)
		mode_flag = BOOT_TYPE_NODISK;
	else
	{
		if(diskless_info.mode_flag)
			mode_flag = BOOT_TYPE_SSD;
		else
			mode_flag = BOOT_TYPE_HARDDISK;
	}
	//report base information.
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_RPTCLIENTINFO, OBJECT_CLIENT);
		pkgSend << gMachine << Client.Mac << Client.Ip << Client.Mark << Client.Gate << Client.DNS << Client.DNS2 << Client.NetSpeed;
		pkgSend << Client.AllArea << Client.ProtArea << Client.Version << Client.ArpRunStatus << mode_flag;
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';

		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';

		pkgRecv >> gServer >> gGameServerName >> gGameServerIp;
		AfxSetSysString(TEXT("config"), TEXT("priorityserver"), gServer);
		AfxSetSysString(TEXT("config"), TEXT("gameserver"), gGameServerName);
		TCHAR szGameServerIp[MAX_PATH] = {0};
		AfxFormatIpString(gGameServerIp, szGameServerIp, MAX_PATH);
		AfxSetSysString(TEXT("config"), TEXT("gameserverip"), szGameServerIp);
		AfxWriteLogger(ghLogger,_T("prioritserver:%s"),gServer);
		AfxWriteLogger(ghLogger,_T("gameserver:%s"),gGameServerName);
		AfxWriteLogger(ghLogger,_T("gameserverip:%s"),szGameServerIp);
		AfxWriteLogger(ghLogger, TEXT("report client info success."));
	}
	catch (int & e)
	{
		bResult = false;
		AfxWriteLogger(ghLogger, TEXT("report client info fail.%d"),e);
	}
	catch (...) 
	{
		bResult = false; 
		AfxWriteLogger(ghLogger, TEXT("report client info fail.")); 
	}
	return bResult;
}
void CTaskServer::WriteShareMemory(LPCTSTR szAgentId,LPCTSTR szNetId)
{
	if(m_hFileMap == NULL)
	{
		CEvenyOneSD sd;
		m_hFileMap = CreateFileMapping(NULL, sd.GetSA(), PAGE_READWRITE, 0, sizeof(ADDVALUE_SHARE_DATA), VALUE_ADD_FILE_MAP_NAME);
	}

	if (m_hFileMap == NULL)
	{
		AfxWriteLogger(ghLogger,TEXT("Create FileMapp Fail."));
		return;
	}

	ADDVALUE_SHARE_DATA* pFileMap = (ADDVALUE_SHARE_DATA*)MapViewOfFile(m_hFileMap, FILE_MAP_READ|FILE_MAP_WRITE, 
		0, 0, sizeof(ADDVALUE_SHARE_DATA));
	if (pFileMap == NULL)
	{
		AfxWriteLogger(ghLogger,TEXT("Map file view error."));
		return;
	}

	pFileMap->dwSignature = ADDVALUE_SHARE_SIG;
	strcpy_s(pFileMap->szAgentId,sizeof(pFileMap->szAgentId),(_bstr_t)szAgentId);
	strcpy_s(pFileMap->szNetbarId,sizeof(pFileMap->szNetbarId),(_bstr_t)szNetId);
	strcpy_s(pFileMap->szMac,sizeof(pFileMap->szMac),(_bstr_t)gMac);

	UnmapViewOfFile(pFileMap);
	pFileMap = NULL;

	AfxWriteLogger(ghLogger,TEXT("write Map file success."));
}

void AfxNetCardId(LPTSTR szNetCard,LPTSTR szDevId, DWORD dwIdSize)
{
	DWORD DeviceIndex = 0;
	SP_DEVINFO_DATA DevData = {sizeof(SP_DEVINFO_DATA)};
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_NET, NULL, NULL, DIGCF_PRESENT);
	while (SetupDiEnumDeviceInfo(hDevInfo, DeviceIndex, &DevData))
	{
		TCHAR szDevName[MAX_PATH] = {0};
		TCHAR szBufferId[MAX_PATH] = {0};
		SetupDiGetDeviceRegistryProperty(hDevInfo,&DevData,SPDRP_HARDWAREID,NULL,(PBYTE)szBufferId, MAX_PATH*sizeof(TCHAR), NULL);
		if(szDevId != NULL && dwIdSize > 0)
			_tcscpy_s(szDevId,dwIdSize,szBufferId);

		if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DevData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)szDevName, MAX_PATH*sizeof(TCHAR), NULL) || szDevName[0] == 0)
		{
			SetupDiGetDeviceRegistryProperty(hDevInfo, &DevData, SPDRP_DEVICEDESC, 0, (PBYTE)szDevName, MAX_PATH*sizeof(TCHAR), NULL);
		}
		if (lstrcmpi(szNetCard,szDevName) == 0)
		{
			SetupDiDestroyDeviceInfoList(hDevInfo);
			hDevInfo = NULL;
			break;
		}
		++DeviceIndex;
	}

	if (hDevInfo)
		SetupDiDestroyDeviceInfoList(hDevInfo);
}

void CTaskServer::ReportHardwareInfo()
{
	tClientInfo Client = {0};

	//get hardware info.
	{
		//AfxGetDevNameByWmi(TEXT("SELECT * FROM Win32_OperatingSystem"), TEXT("Caption"), Client.OS, _countof(Client.OS));
		DWORD dwRegType = REG_SZ, dwSize = sizeof(Client.OS);
		SHGetValue(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), TEXT("ProductName"),
			&dwRegType, &Client.OS, &dwSize);

		if(m_bInitHw && m_pGetMainBoardNameInfo)
			m_pGetMainBoardNameInfo(Client.MainBoard,_countof(Client.MainBoard));
		//		AfxGetDevNameByWmi(TEXT("SELECT * FROM Win32_BaseBoard"), TEXT("Product"), Client.MainBoard, _countof(Client.MainBoard));
		// 		dwRegType = REG_SZ, dwSize = sizeof(Client.MainBoard);
		// 		SHGetValue(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\System\\BIOS"), TEXT("BaseBoardProduct"),
		// 			&dwRegType, &Client.MainBoard, &dwSize);

		if(m_bInitHw && m_pGetRamInfo)
			m_pGetRamInfo(Client.Memory,_countof(Client.Memory));
		// 		std::vector<tstring> MemoryList;
		// 		AfxGetDevNameByWmi(TEXT("SELECT * FROM Win32_PhysicalMemory"), TEXT("Capacity"), Client.Memory, _countof(Client.Memory));
		// 		AfxSplitString(Client.Memory, MemoryList);
		// 		__int64 qMemorySize = 0;
		// 		for (std::vector<tstring>::iterator it = MemoryList.begin(); it != MemoryList.end(); it++)
		// 		{
		// 			qMemorySize += _ttoi64(it->c_str());
		// 		}
		// 		qMemorySize /= (1024 * 1024 * 1024);
		// 		if (qMemorySize == 0)
		// 		{
		// 			MEMORYSTATUSEX MemStatus = { sizeof(MEMORYSTATUSEX) };
		// 			if(GlobalMemoryStatusEx(&MemStatus))
		// 			{
		// 				qMemorySize = MemStatus.ullTotalPhys / (1024*1024);
		// 				qMemorySize = (qMemorySize + 1023) / 1024;
		// 			}
		// 		}
		// 		_stprintf_s(Client.Memory, _countof(Client.Memory), TEXT("%dG"), (DWORD)qMemorySize);

		AfxNetCardId(gNetCard,Client.NetCardId,_countof(Client.NetCardId));
		lstrcpy(Client.NetCard, gNetCard);
		AfxGetFirstDevName(GUID_DEVCLASS_PROCESSOR, Client.Cpu, _countof(Client.Cpu));
		AfxGetFirstDevName(GUID_DEVCLASS_IMAGE, Client.Camera, _countof(Client.Camera));
		AfxGetFirstDevName(GUID_DEVCLASS_DISKDRIVE, Client.Disk, _countof(Client.Disk),Client.DiskId,_countof(Client.DiskId));
		if(m_bInitHw && m_pGetDisply)
		{
			DISPLAYADAPTER dispapter = {0};
			if(m_pGetDisply(&dispapter,0))
			{
				lstrcpy(Client.Video, dispapter.szModel);
				lstrcpy(Client.VideoId, dispapter.szModel);
			}
		}
	}

	//report hardware info.
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_RPTHARDINFO, OBJECT_CLIENT);
		pkgSend << gMachine << Client.OS << Client.Cpu << Client.MainBoard << Client.Memory << Client.Disk << Client.Video << Client.NetCard << Client.Camera;
		pkgSend << Client.Explorer << Client.JHSoft << Client.WPSoft << Client.DiskId << Client.VideoId << Client.NetCardId;
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		AfxWriteLogger(ghLogger, TEXT("report hardware info success."));
	}
	catch (int & e) { AfxWriteLogger(ghLogger, TEXT("report hardware info fail. %d"),e); }
	catch (...) { AfxWriteLogger(ghLogger, TEXT("report hardware info fail."));}
}

void CTaskServer::ReportTemprature()
{
	if (1 != GetSysOptInteger(OPT_M_TEMPERATUREENABLE))
		return ;

	try
	{
		tTemperature Temperature = {0};
		if(m_bInitHw && m_pGetTemperature && m_pGetTemperature(&Temperature))
		{
			char szSendBuff[PKG_MAXLENGTH] = {0}, szRecvBuff[PKG_MAXLENGTH] = {0};
			CPkgHelper pkg(szSendBuff, CMD_CLI_RPTTEMPRATURE, OBJECT_CLIENT);
			pkg << gMachine << Temperature.CpuVal << Temperature.BoardVal << Temperature.DisplayVal << Temperature.DiskVal << Temperature.CpuFanSpeed;
			AfxSocketExecuteLock(gSocket, szSendBuff, pkg.GetLength(), szRecvBuff, sizeof(szRecvBuff));
		}
	}
	catch (...) {}	
}
bool CTaskServer::ReadDataFile(LPCTSTR szFileName, LPSTR& pData, DWORD& dwLength)
{
	TCHAR szFullFileName[MAX_PATH] = {0};
	if (lstrlen(szFileName) > 3 && szFileName[1] != TEXT(':') && szFileName[2] != TEXT('\\'))
		AfxGetAppPath(szFullFileName, MAX_PATH, szFileName);
	else
		lstrcpy(szFullFileName, szFileName);

	HANDLE hFile = CreateFile(szFullFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwSize = GetFileSize(hFile, NULL);
	if(dwSize == 0)
	{
		CloseHandle(hFile);
		return false;
	}
	pData = (char*)CoTaskMemAlloc(dwSize + 1);
	DWORD dwReadBytes = 0;
	if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwReadBytes != dwSize)
	{
		AfxWriteLogger(ghLogger, TEXT("read open %s file fail! error code:%d."), szFullFileName, GetLastError());
		CloseHandle(hFile);
		CoTaskMemFree(pData);
		return false;
	}
	dwLength = dwReadBytes;
	pData[dwReadBytes] = 0;
	CloseHandle(hFile);
	return true;
}
bool CTaskServer::WriteDataFile(LPCTSTR szFileName, LPVOID pData, int length)
{
	TCHAR szFullFileName[MAX_PATH] = {0};
	if (lstrlen(szFileName) > 3 && szFileName[1] != TEXT(':') && szFileName[2] != TEXT('\\'))
		AfxGetAppPath(szFullFileName, MAX_PATH, szFileName);
	else
		lstrcpy(szFullFileName, szFileName);
	SetFileAttributes(szFullFileName, FILE_ATTRIBUTE_NORMAL);
	HANDLE hFile = ::CreateFile(szFullFileName, FILE_ALL_ACCESS, FILE_SHARE_WRITE | FILE_SHARE_READ, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwWrite = 0;
		WriteFile(hFile, pData, length, &dwWrite, NULL);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
		hy_PortFile(szFullFileName);
		return true;
	}
	return false;
}
void CTaskServer::DownLoadMenuBg()
{
	tstring strCustomBg = GetSysOptString(OPT_M_CUSTOMMENUBG);
	std::vector<tstring> strSplit;
	kcore::AfxSplitString(strCustomBg,strSplit,_T('|'));
	for(size_t i=0;i<strSplit.size();i++)
	{
		CString strFileName = strSplit[i].c_str();
		CString strSrcFile = _T("config\\icon\\");
		strSrcFile += strFileName;
		TCHAR szImage[MAX_PATH] = {0};
		AfxGetAppPath(szImage, MAX_PATH, _T("Skin\\Image\\"));
		lstrcat(szImage, strFileName);
		if(!PathFileExists(szImage))
		{
			if (!DownLoadFile(strSrcFile, szImage, -1, -1, TRUE))
				AfxWriteLogger(ghLogger, TEXT("down menu bg file fail:%s"), szImage);
		}
	}
}
void CTaskServer::GetSystemOption()
{
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETSYSOPTION, OBJECT_CLIENT);
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		DWORD dwSize = 0;
		pkgRecv >> dwSize;
		for (DWORD i=0; i<dwSize; ++i)
		{
			TCHAR szKey[MAX_PATH] = {0}, szValue[4096] = {0};
			pkgRecv >> szKey >> szValue;
			gSysOption.insert(std::make_pair(szKey, szValue));
			if (_tcscmp(szKey, TEXT("mrunmenu")) == 0)
			{
				if (_ttoi(szValue) == 1)
				{
					needmenu = true;
				}
			}

		}

		std::pair<LPCTSTR, LPCTSTR> optarray[] = 
		{
			std::make_pair(OPT_U_USERID,		TEXT("nbid")),
			std::make_pair(OPT_U_AGENTID,		TEXT("agid")),
			std::make_pair(OPT_U_NETBARNAME,	TEXT("netbarname")),
		};
		for (size_t index=0; index<_countof(optarray); index++)
		{
			tstring Value = GetSysOptString(optarray[index].first);
			SHSetValue(HKEY_LOCAL_MACHINE, SOFTWARE_KEY, optarray[index].second, REG_SZ, Value.c_str(), (DWORD)((Value.size() + 1) * sizeof(TCHAR)));
		}

		/*tstring Value = GetSysOptString(OPT_M_ASSISTENABLE,_T("0"));
		DWORD dwValue = _ttol(Value.c_str());
		SHSetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet"), TEXT("assist"), REG_DWORD, &dwValue, sizeof(DWORD));*/

		WriteDataFile(TEXT("config\\koption.dat"), RecvBuffer, pkgRecv.GetLength());
		AfxWriteLogger(ghLogger, TEXT("query system option success."));

		WriteShareMemory(GetSysOptString(OPT_U_AGENTID).c_str(),GetSysOptString(OPT_U_USERID).c_str());
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, TEXT("query system option fail. %d"),e);
	}
	catch (...)
	{
		AfxWriteLogger(ghLogger, TEXT("query system option fail."));
	}
}

void CTaskServer::GetServerList()
{
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETSERVERLIST, OBJECT_CLIENT);
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		DWORD dwSize = 0;
		pkgRecv >> dwSize;
		for (DWORD i=0; i<dwSize; ++i)
		{
			TCHAR szServer[MAX_PATH] = {0}, szIp[MAX_PATH] = {0};
			DWORD dwIp = 0;
			pkgRecv >> szServer >> dwIp;

			_stprintf_s(szIp, _countof(szIp), TEXT("%d"), dwIp);
			std::map<tstring, tstring>::iterator it = gSvrList.find(szServer);
			if (it == gSvrList.end())
				gSvrList.insert(std::make_pair(szServer, szIp));
			else
			{
				tstring ip = it->second;
				_stprintf_s(szIp, _countof(szIp), TEXT("%s|%d"), ip.c_str(), dwIp);
				it->second = szIp;
			}
		}
		WriteDataFile(TEXT("config\\ksvrlist.dat"), RecvBuffer, pkgRecv.GetLength());
		AfxWriteLogger(ghLogger, TEXT("query server list success."));
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, TEXT("query server list fail.%d"),e);
	}
	catch (...)
	{
		AfxWriteLogger(ghLogger, TEXT("query server list fail."));
	}
}
void CTaskServer::GetServerListEx()
{
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_QUERYALLGAMESVR, OBJECT_CLIENT);
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		DWORD dwSize = 0;
		pkgRecv >> dwSize;
		for (DWORD i=0; i<dwSize; ++i)
		{
			tagServerList tagSvrList;
			pkgRecv >> tagSvrList.Id 
				>> tagSvrList.ip
				>> tagSvrList.flag;

			gSvrListEx.push_back(tagSvrList);
		}
		WriteDataFile(TEXT("config\\ksvrlist.dat"), RecvBuffer, pkgRecv.GetLength());
		AfxWriteLogger(ghLogger, TEXT("query server list success."));
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, TEXT("query server list fail.%d"),e);
	}
	catch (...)
	{
		AfxWriteLogger(ghLogger, TEXT("query server list fail."));
	}
}
void CTaskServer::GetRateInfo()
{
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_QUERYRATE, OBJECT_CLIENT);
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';

		WriteDataFile(TEXT("config\\krate.dat"), RecvBuffer, pkgRecv.GetLength());
		AfxWriteLogger(ghLogger, TEXT("query rate success."));
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, TEXT("query rate fail.%d"),e);
	}
	catch (...)
	{
		AfxWriteLogger(ghLogger, TEXT("query rate fail."));
	}
}
void CTaskServer::GetVdiskList()
{
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETVDISKLIST, OBJECT_CLIENT);
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		DWORD dwSize = 0;
		pkgRecv >> dwSize;
		for (DWORD i=0; i<dwSize; ++i)
		{
			tDiskMgr Disk = {0};
			pkgRecv >> Disk.SERVER >> Disk.DiskDriver >> Disk.VdiskLetter;
			gDiskMgr.push_back(Disk);
		}
		WriteDataFile(TEXT("config\\kvdisk.dat"), RecvBuffer, pkgRecv.GetLength());
		ghVidskThread = (HANDLE)_beginthreadex(NULL, 0, VdiskWorkThread, this, 0, NULL);
		AfxWriteLogger(ghLogger, TEXT("query vdisk list success."));
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, TEXT("query vdisk list fail.%d"),e);
	}
	catch (...)
	{
		AfxWriteLogger(ghLogger, TEXT("query vdisk list fail."));
	}
}

void CTaskServer::GetVdiskListEx()
{
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_QUERYALLGAMESVRDRIVER, OBJECT_CLIENT);
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		DWORD dwSize = 0;
		pkgRecv >> dwSize;
		for (DWORD i=0; i<dwSize; ++i)
		{
			tagAllServerGameDisk tagGamedisk;
			pkgRecv >> tagGamedisk.ip 
				>> tagGamedisk.diskdriver
				>> tagGamedisk.vdiskletter;

			gDiskMgrEx.push_back(tagGamedisk);
		}

		WriteDataFile(TEXT("config\\kvdisk.dat"), RecvBuffer, pkgRecv.GetLength());
		ghVidskThread = (HANDLE)_beginthreadex(NULL, 0, VdiskWorkThreadEx, this, 0, NULL);
		AfxWriteLogger(ghLogger, TEXT("query vdisk list success."));
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, TEXT("query vdisk list fail.%d"),e);
	}
	catch (...)
	{
		AfxWriteLogger(ghLogger, TEXT("query vdisk list fail."));
	}
}

void CTaskServer::WriteGameInfoRegist()
{
	SHDeleteKey(HKEY_LOCAL_MACHINE, SOFTKEY_GINFO);

	for (std::map<DWORD, tGameInfo>::iterator it = gGameInfo.begin(); it != gGameInfo.end(); it++)
	{
		tGameInfo g = it->second;

		TCHAR key[MAX_PATH], info[MAX_PATH * 2] = {0};
		_stprintf_s(key, _countof(key), TEXT("%d"), g.gid);
		_stprintf_s(info, _countof(info), TEXT("%s|%s|%s|%d"), g.gname, g.gsvrpath, g.gclipath, g.runtype);
		SHSetValue(HKEY_LOCAL_MACHINE, SOFTKEY_GINFO, key, REG_SZ, info, (DWORD)_tcslen(info) * sizeof(TCHAR));
	}

}
void CTaskServer::WriteGamesInfo(std::map<DWORD, tGameInfo>& GameInfo)
{
	DWORD lLength = (DWORD)ROUND_SIZE_64K(sizeof(PkgHeader) + sizeof(DWORD) + GameInfo.size() * sizeof(tGameInfo));
	char* pBuffer = new char[lLength];
	CPkgHelper  pkg(pBuffer, lLength, CMD_CLI_GETGAMELIST, OBJECT_CLIENT);
	pkg << (DWORD)GameInfo.size();
	for (std::map<DWORD, tGameInfo>::iterator it = GameInfo.begin(); it != GameInfo.end(); it++)
	{
		tGameInfo g = it->second;
		pkg	<< g.gid << g.cid << g.Class << g.gname << g.galias << g.grungame << g.gexefile << g.gparameter << g.gsvrpath << g.gclipath << g.gprogress;
		pkg	<< g.gsize << g.gtoolbar << g.gpush <<g.desklnk << g.toolbar << g.svrver << g.svrclick << g.invalue << g.runtype << g.hottag << g.Idcclick;
		pkg << g.gConfig << g.idcdate << g.nWait;
	}

	WriteDataFile(TEXT("config\\kgame.dat"), pBuffer, pkg.GetLength());
	delete [] pBuffer;
}

void CTaskServer::GetGameInfo()
{
	AfxWriteLogger(ghLogger, TEXT("start query game list."));
	const int SINGLE_TIME_GAME_COUNT = 100;
	try
	{
		std::vector<DWORD> gamelist;
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETGAMELIST, OBJECT_CLIENT);
		pkgSend << gMachine;
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		DWORD dwSize = 0;
		pkgRecv >> dwSize;
		for (DWORD i=0; i<dwSize; ++i)
		{
			DWORD gid = 0;
			pkgRecv >> gid;
			gamelist.push_back(gid);
		}

		size_t nTimes = (gamelist.size() + SINGLE_TIME_GAME_COUNT - 1) / SINGLE_TIME_GAME_COUNT;
		for (size_t idx=0; idx<nTimes; idx++)
		{
			char  szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
			CPkgHelper pkgSend(szSendBuffer, CMD_CLI_GETGAMEINFOEX, OBJECT_CLIENT);

			DWORD dwCount = 0, offset = 0;
			pkgSend.PushString(gMachine);
			offset = pkgSend.GetOffset();
			pkgSend.Push(dwCount);
			for (size_t loop=0; loop<SINGLE_TIME_GAME_COUNT; loop++)
			{
				size_t index = idx*SINGLE_TIME_GAME_COUNT + loop;
				if (index >= gamelist.size())
					break;
				pkgSend << gamelist[index];
				dwCount++;
			}
			pkgSend.Push(offset, dwCount);
			if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, szSendBuffer, pkgSend.GetLength(), szRecvBuffer, sizeof(szRecvBuffer)))
				throw (int)'ee';
			CPkgHelper pkgRecv(szRecvBuffer);
			if (!pkgRecv.IsStatusOk())
				throw (int)'ww';
			pkgRecv >> dwSize;
			for (DWORD i=0; i<dwSize; ++i)
			{
				tGameInfo g = {0};
				pkgRecv	>> g.gid >>g.cid >> g.Class >> g.gname >> g.galias >> g.grungame >> g.gexefile >> g.gparameter >> g.gsvrpath >> g.gclipath >> g.gprogress;
				pkgRecv	>> g.gsize >> g.gtoolbar >> g.gpush >>g.desklnk >> g.toolbar >> g.svrver >> g.svrclick >> g.invalue >> g.runtype >> g.hottag >> g.Idcclick;
				pkgRecv >> g.gConfig >> g.idcdate>>g.nWait;
				gGameInfo.insert(std::make_pair(g.gid,g));
				if (g.gpush)
					gPushInfo.insert(std::make_pair(g.gid, g));
			}
		}
		WriteGamesInfo(gGameInfo);
		ghPushThread = (HANDLE)_beginthreadex(NULL, 0, PushGameThread, this, 0, NULL);
		AfxWriteLogger(ghLogger, TEXT("query game list success."));
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, TEXT("query game list fail. %d"),e);
	}
	catch (...)
	{
		AfxWriteLogger(ghLogger, TEXT("query game list fail."));
	}
}
void CTaskServer::GetGameConfig()
{
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_QUERYGAMECONFIG, OBJECT_CLIENT);
		pkgSend << gMachine;
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		DWORD dwSize = 0;
		pkgRecv >> dwSize;
		for (DWORD i=0; i<dwSize; ++i)
		{
			tGameConfig gameconfig = {0};
			pkgRecv >> gameconfig.gid >> gameconfig.gameconfig >> gameconfig.szFileName;
			gGameConfig.push_back(gameconfig);
		}
		AfxWriteLogger(ghLogger, TEXT("query game config success."));
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, TEXT("query game config fail.%d"),e);
	}
	catch (...)
	{
		AfxWriteLogger(ghLogger, TEXT("query game config fail."));
	}
}
bool CTaskServer::GetNeedUpdateIconList(std::set<DWORD>& gids)
{
	//get server icon index..
	char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
	CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETICONLIST, OBJECT_CLIENT);

	if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
		return false;
	CPkgHelper pkgRecv(RecvBuffer);
	if (!pkgRecv.IsStatusOk())
		return false;

	DWORD dwSize = 0;
	CPkgHelper pkgOut(RecvBuffer);
	pkgOut >> dwSize;
	for (DWORD loop=0; loop<dwSize; loop++)
	{
		tagIconInfo iconInfo = {0};
		pkgOut >> iconInfo.dwGid >> iconInfo.dwHash;
		//大于100000也是需要下载的
		if (gGameInfo.find(iconInfo.dwGid) != gGameInfo.end() || iconInfo.dwGid > 100000)
		{
			gIconInfo[iconInfo.dwGid] = iconInfo;
			gids.insert(iconInfo.dwGid);
		}
	}
	//load local disk icon info.
	{
		LPSTR pData = NULL;
		DWORD dwLength = 0;
		if(ReadDataFile(_T("config\\kicons.dat"), pData, dwLength))
		{
			char* pBuffer = NULL;
			try
			{
				CPkgHelper pkgOut(pData,dwLength,'t','a');				
				DWORD dwSize = 0;
				pkgOut.Pop(dwSize);
				for (DWORD i=0; i<dwSize; ++i)
				{
					tagIconInfo iconInfo = {0};
					pkgOut >> iconInfo.dwGid >> iconInfo.dwHash;
					pkgOut >> iconInfo.dwLength;
					if (iconInfo.dwLength > 0)
					{
						iconInfo.pIcon = new char[iconInfo.dwLength];
						pBuffer = iconInfo.pIcon;
						pkgOut.PopPointer(iconInfo.pIcon, iconInfo.dwLength);
						pBuffer = NULL;
					}

					std::map<DWORD, tagIconInfo>::iterator it = gIconInfo.find(iconInfo.dwGid);
					if (it != gIconInfo.end() && iconInfo.dwHash == it->second.dwHash && iconInfo.dwHash != 0)
					{
						gids.erase(iconInfo.dwGid);
						gIconInfo[iconInfo.dwGid] = iconInfo;
					}
				}
			}
			catch(...)
			{
				AfxWriteLogger(ghLogger,_T("load local disk icon info exception."));
				if(pBuffer)
				{
					delete pBuffer;
					pBuffer = NULL;
				}
			}
			CoTaskMemFree(pData);
		}
	}
	return true;
}



bool CTaskServer::GetIconInfoByGid(SOCKET  so,CPkgHelper& pkgIn)
{
	char  RecvBuffer[PKG_MAXLENGTH] = { 0 };
	if (ERROR_SUCCESS != AfxSocketExecuteLock(so, (char*)pkgIn.GetBuffer(), pkgIn.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
		return false;
	CPkgHelper pkgRecv(RecvBuffer);
	if (!pkgRecv.IsStatusOk())
		return false;


	tagIconInfo iconInfo = { 0 };
	pkgRecv >> iconInfo.dwGid;
	pkgRecv >> iconInfo.dwHash;
	pkgRecv >> iconInfo.dwLength;
	std::map<DWORD, tagIconInfo>::iterator it = gIconInfo.find(iconInfo.dwGid);
	if (it != gIconInfo.end())
	{
		char* pData = it->second.pIcon;
		if (pData != NULL)
		{
			delete[] pData;
			pData = NULL;
		}
	}

	if (iconInfo.dwLength > 0)
	{
		iconInfo.pIcon = new char[iconInfo.dwLength + 1];
		pkgRecv.PopPointer(iconInfo.pIcon, iconInfo.dwLength);
		gIconInfo[iconInfo.dwGid] = iconInfo;
	}
	return true;
}

bool CTaskServer::GetIconInfoByGid(CPkgHelper& pkgIn)
{
	char  RecvBuffer[PKG_MAXLENGTH] = {0};
	if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, (char*)pkgIn.GetBuffer(), pkgIn.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
		return false;
	CPkgHelper pkgRecv(RecvBuffer);
	if (!pkgRecv.IsStatusOk())
		return false;


	tagIconInfo iconInfo = {0};
	pkgRecv >> iconInfo.dwGid;
	pkgRecv >> iconInfo.dwHash;
	pkgRecv >> iconInfo.dwLength;
	std::map<DWORD, tagIconInfo>::iterator it = gIconInfo.find(iconInfo.dwGid);
	if (it != gIconInfo.end())
	{
		char* pData = it->second.pIcon;
		if (pData != NULL)
		{
			delete [] pData;
			pData = NULL;
		}
	}

	if (iconInfo.dwLength > 0)
	{
		iconInfo.pIcon = new char[iconInfo.dwLength + 1];
		pkgRecv.PopPointer(iconInfo.pIcon, iconInfo.dwLength);
		gIconInfo[iconInfo.dwGid] = iconInfo;
	}
	return true;
}
void CTaskServer::WriteIconData()
{
	AfxWriteLogger(ghLogger,TEXT("write data to kicons.dat"));
	bool bProtected = false;
	TCHAR szFullFileName[MAX_PATH] = {0};
	AfxGetAppPath(szFullFileName, MAX_PATH, _T("config\\kicons.dat")); ;
	HANDLE hFile = ::CreateFile(szFullFileName, FILE_ALL_ACCESS, FILE_SHARE_WRITE | FILE_SHARE_READ, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = GetLastError();
		_com_error Error(dwError);
		AfxWriteLogger(ghLogger,TEXT("kicons.dat Create File Fail:%d:%s!"), dwError, Error.ErrorMessage());
		return ;
	}

	static bool bHeader = true;
	DWORD dwSize = (DWORD)gIconInfo.size();
	std::map<DWORD, tagIconInfo>::iterator iter = gIconInfo.begin();
	for (iter; iter != gIconInfo.end(); ++iter)
	{
		long lOffset = 0;
		char szBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper  pkgIn(szBuffer, PKG_MAXLENGTH, CMD_CLI_GETICONINFO, OBJECT_CLIENT);
		if (bHeader)
		{
			pkgIn.Push(dwSize);
			bHeader = false;
		}
		else
		{
			lOffset = sizeof(PkgHeader);
		}
		pkgIn.Push(iter->second.dwGid);
		pkgIn.Push(iter->second.dwHash);
		pkgIn.Push(iter->second.dwLength);
		pkgIn.PushPointer(iter->second.pIcon, iter->second.dwLength);

		DWORD dwWrite = 0;
		if (!WriteFile(hFile, szBuffer + lOffset, pkgIn.GetLength() - lOffset, &dwWrite, NULL) 
			|| dwWrite != (pkgIn.GetLength() - lOffset))
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			AfxWriteLogger(ghLogger,TEXT("kicons.dat Write File Fail:%d:%s!"), dwError, Error.ErrorMessage());
		}

		SetFilePointer(hFile,0,NULL,FILE_CURRENT);
	}

	SetEndOfFile(hFile);
	CloseHandle(hFile);

	hy_PortFile(szFullFileName);
}

void CTaskServer::MutiThreadGetIconInfo()
{	
	DownIconCount= AfxGetSysLong(L"config",L"thicon",2);
	for (int i = 0; i < DownIconCount; i++)
	{
		std::thread * th = new std::thread(&CTaskServer::DownGetIconInfo, this);
		threadpool.push_back(th);
	}
}
void CTaskServer::DownGetIconInfo()
{	
	TCHAR szServerIp[MAX_PATH] = { 0 };
	AfxGetSysString(TEXT("config"), TEXT("serverip"), szServerIp, _countof(szServerIp), TEXT("127.0.0.1"));
	SOCKET tmpSocket = AfxSocketConnect(szServerIp, APP_TCP_PORT);
	if (tmpSocket == INVALID_SOCKET)
	{
		AfxWriteLogger(ghLogger, TEXT("in icon connect %s fail."), szServerIp);
		return;
	}
	while (!gidss.empty())
	{
		DWORD gid = 0;
		{
			std::lock_guard<std::mutex> lock(mtx);
			if (gidss.empty())
				return;
			gid = *gidss.begin();
			gidss.erase(gidss.begin());
		}
		if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
		{
			AfxSocketClose(tmpSocket);
			return;
		}
		char szSendBuffer[PKG_MAXLENGTH] = { 0 };
		CPkgHelper  pkgIn(szSendBuffer, CMD_CLI_GETICONINFO, OBJECT_CLIENT);
		pkgIn << gid;				
		GetIconInfoByGid(tmpSocket, pkgIn);
	}
	AfxSocketClose(tmpSocket);
}
void CTaskServer::GetIconInfo()
{
	/*TCHAR szIconDstFile[MAX_PATH] = {0};
	AfxGetAppPath(szIconDstFile, MAX_PATH, TEXT("config\\kicon.dat"));
	SetFileAttributes(szIconDstFile, FILE_ATTRIBUTE_NORMAL);
	bool bRet = DownLoadFile(TEXT("config\\icon15.db"), szIconDstFile, -1, -1, TRUE);
	if (!bRet)
	bRet = DownLoadFile(TEXT("config\\icon15.db"), szIconDstFile, -1, -1, TRUE);
	if (bRet)
	hy_PortFile(szIconDstFile);
	AfxWriteLogger(ghLogger, TEXT("query icon list %s."), bRet ? TEXT("success") : TEXT("fail"));*/
	AfxWriteLogger(ghLogger, TEXT("start query icon info."));
	std::set<DWORD> gids;
	if (!GetNeedUpdateIconList(gids))
	{
		AfxWriteLogger(ghLogger, TEXT("GetNeedUpdateIconList ret:%d"), gids.size());
		return;
	}
	gidss.clear();
	gidss = gids;
	MutiThreadGetIconInfo();
	for (int i = 0; i < DownIconCount; i++)
	{
		threadpool[i]->join();
	}
	for (int i = 0; i < DownIconCount; i++)
	{
		delete threadpool[i];
	}

	/*

	std::set<DWORD>::iterator iter = gids.begin();
	for (;iter != gids.end(); ++iter)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
			return ;
		char szSendBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper  pkgIn(szSendBuffer, CMD_CLI_GETICONINFO, OBJECT_CLIENT);
		pkgIn << *iter;
		GetIconInfoByGid(pkgIn);
	}
	*/
	if (gids.size())
		WriteIconData();

	AfxWriteLogger(ghLogger,TEXT("query icon info succeed."));

}
void CTaskServer::SaveDialogInfo()
{
	char buf[PKG_MAXLENGTH] = {0};
	CPkgHelper pkg(buf,OBJECT_UNKOWN,OBJECT_UNKOWN);
	pkg.Push((DWORD)gDialog.size());
	for (size_t i=0;i<gDialog.size();i++)
		pkg << gDialog[i];

	pkg.Push((DWORD)gItemShortCut.size());
	for (size_t i=0;i<gItemShortCut.size();i++)
		pkg << gItemShortCut[i];

	WriteDataFile(CLIENT_DATA_DIALOG, buf, pkg.GetLength());
}

void CTaskServer::LoadDialogInfo()
{
	gDialog.clear();
	gItemShortCut.clear();
	char* pData = NULL;
	DWORD dwLength = 0;
	if(ReadDataFile(CLIENT_DATA_DIALOG, pData, dwLength))
	{
		CPkgHelper pkg(pData);
		DWORD dwSize = 0;
		pkg.Pop(dwSize);
		for (DWORD i=0; i<dwSize; ++i)
		{
			DIALOG_INFO dialog = {0};
			pkg >> dialog;
			gDialog.push_back(dialog);
		}
		pkg.Pop(dwSize);
		for (DWORD i=0; i<dwSize; ++i)
		{
			ItemShortCut item = {0};
			pkg >> item;
			gItemShortCut.push_back(item);
		}
	}
	CoTaskMemFree(pData);
}
void CTaskServer::GetDeskTopLinkPath(LPTSTR szDesktop)
{
	TCHAR szKey[MAX_PATH] = {0}, szLink[MAX_PATH] = {0}, szTemp[MAX_PATH] = {0};
	_stprintf_s(szKey, _countof(szKey), TEXT("%s\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), 
		AfxGetCurUserHKeyRoot(szTemp, _countof(szTemp)));
	DWORD dwType = REG_SZ, dwSize = _countof(szLink);
	SHGetValue(HKEY_USERS, szKey, TEXT("Desktop"), &dwType, szLink, &dwSize);
	if(_tcslen(szLink) == 0)
	{
		HANDLE hToken = NULL;
		HANDLE hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, AfxGetProcIdByName(TEXT("explorer.exe")));
		if (hProcess != NULL)
		{
			if(OpenProcessToken (hProcess, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken))
			{
				SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, hToken, SHGFP_TYPE_CURRENT, szLink);
			}
			CloseHandle(hProcess);
		}
	}
	PathAddBackslash(szLink);
	lstrcpy(szDesktop, szLink);
}

bool CTaskServer::CreateDesktopLink(LPCTSTR name, LPCTSTR exe, LPCTSTR param, LPCTSTR icon, LPCTSTR memo, bool &bRefresh)
{
	for (size_t i=0;i<gItemShortCut.size();i++)
	{
		if(_tcsicmp(name,gItemShortCut[i].strName) == 0)
		{
			TCHAR szWork[MAX_PATH] = {0};
			lstrcpy(szWork, exe); PathRemoveFileSpec(szWork); PathAddBackslash(szWork);
			_tcscpy_s(gItemShortCut[i].strExe , exe);
			_tcscpy_s(gItemShortCut[i].strIconPath , icon);
			_tcscpy_s(gItemShortCut[i].strMemo , memo);
			_tcscpy_s(gItemShortCut[i].strParam , param);
			_tcscpy_s(gItemShortCut[i].strWorkDir , szWork);
			return true;
		}
	}
	// 创建三次快捷方式
	for (int i = 0; i < 3; ++i)
	{
		TCHAR szLink[MAX_PATH] = {0}, szWork[MAX_PATH] = {0};

		if (i == 0)
		{
			// 第一次使用用户注册表路径
			GetDeskTopLinkPath(szLink);
		}
		else if (i == 1)
		{
			// 第二次使用注册表路径
			DWORD dwType = REG_SZ, dwSize = _countof(szLink);
			SHGetValue(HKEY_USERS, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), TEXT("Desktop"), &dwType, szLink, &dwSize);
			if (szLink[0] == 0)
			{
				continue;
			}
			PathAddBackslash(szLink);
		}
		else if (i == 2)
		{
			// 第三次使用系统函数获取路径
			HANDLE hToken = NULL;
			HANDLE hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, AfxGetProcIdByName(TEXT("explorer.exe")));
			if (hProcess != NULL)
			{
				if(OpenProcessToken (hProcess, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken))
				{
					SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, hToken, SHGFP_TYPE_CURRENT, szLink);
				}
				CloseHandle(hProcess);
			}
			if (szLink[0] == 0)
			{
				break;
			}
			PathAddBackslash(szLink);
		}

		lstrcat(szLink, name); lstrcat(szLink, TEXT(".lnk"));
		lstrcpy(szWork, exe); PathRemoveFileSpec(szWork); PathAddBackslash(szWork);

		IShellLink*   pIShellLink = NULL;
		IPersistFile* pIPersistFile = NULL;
		HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&pIShellLink);
		if(SUCCEEDED(hr))
		{
			hr = pIShellLink->QueryInterface(IID_IPersistFile, (void**)&pIPersistFile);
			if (SUCCEEDED(hr))
			{
				BOOL bCreate = TRUE;
				if (SUCCEEDED(pIPersistFile->Load(szLink, STGM_READWRITE)))
				{
					TCHAR szOldWork[MAX_PATH] = {0}, szOldExe[MAX_PATH] = {0}, szOldParam[MAX_PATH] = {0};
					TCHAR szOldIcon[MAX_PATH] = {0}, szOldMemo[MAX_PATH] = {0}; 
					int nIndex = 0;
					WIN32_FIND_DATA wfd = {0};
					HRESULT hr = 0;
					hr = pIShellLink->GetWorkingDirectory(szOldWork, MAX_PATH);
					hr = pIShellLink->GetPath(szOldExe, MAX_PATH, &wfd, SLGP_UNCPRIORITY);
					hr = pIShellLink->GetArguments(szOldParam, MAX_PATH);
					hr = pIShellLink->GetIconLocation(szOldIcon, MAX_PATH, &nIndex);
					hr = pIShellLink->GetDescription(szOldMemo, MAX_PATH);

					if (_tcsicmp(szWork, szOldWork) == 0 &&  _tcsicmp(exe, szOldExe) == 0 && _tcsicmp(param, szOldParam) == 0 &&
						_tcsicmp(icon, szOldIcon) == 0 && _tcsicmp(memo, szOldMemo) == 0)
					{
						bCreate = FALSE;
					}
				}
				if (bCreate)
				{
					SetFileAttributes(szLink, FILE_ATTRIBUTE_NORMAL);
					DeleteFile(szLink);
					pIShellLink->SetPath(exe);
					pIShellLink->SetWorkingDirectory(szWork);
					pIShellLink->SetIconLocation(icon, 0);
					pIShellLink->SetArguments(param);
					pIShellLink->SetDescription(memo);
					HRESULT hr = pIPersistFile->Save(szLink, TRUE);
					bRefresh = true;
				}
				pIPersistFile->Release();
				AfxWriteLogger(ghLogger, TEXT("create desktop link:%s"), szLink);
			}
			pIShellLink->Release();
		}
	}

	return true;
}

bool CTaskServer::DeleteMoreLink(std::map<tstring, tstring>& LinkList)
{
	std::vector<int> arDeleItem;
	for (size_t i=0;i<gItemShortCut.size();i++)
	{
		std::map<tstring, tstring>::iterator iter = LinkList.find(gItemShortCut[i].strMemo);
		if (_tcsnicmp(gItemShortCut[i].strMemo, TEXT("sig="), 4) == 0 && 
			(iter== LinkList.end() || 
			_tcsicmp(iter->second.c_str(),gItemShortCut[i].strName) != 0))
		{
			arDeleItem.push_back((int)i);
		}
	}

	for (int i = (int)(arDeleItem.size() - 1); i >= 0; i--)
		gItemShortCut.erase(gItemShortCut.begin()+arDeleItem[i]);

	bool bResult = false;
	TCHAR szDesktop[MAX_PATH] = {0};
	GetDeskTopLinkPath(szDesktop);

	TCHAR szPath[MAX_PATH] = {0};
	_stprintf_s(szPath, _countof(szPath), TEXT("%s*.lnk"), szDesktop);
	WIN32_FIND_DATA wfd = {0};
	HANDLE hFinder = FindFirstFile(szPath, &wfd);
	if (hFinder != INVALID_HANDLE_VALUE)
	{
		do
		{
			TCHAR szLinkFile[MAX_PATH] = {0};
			_stprintf_s(szLinkFile, _countof(szLinkFile), TEXT("%s%s"), szDesktop, wfd.cFileName);

			IShellLink*   pIShellLink = NULL;
			IPersistFile* pIPersistFile = NULL;
			HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&pIShellLink);
			if(SUCCEEDED(hr))
			{
				hr = pIShellLink->QueryInterface(IID_IPersistFile, (void**)&pIPersistFile);
				if(SUCCEEDED(hr))
				{
					pIPersistFile->Load(szLinkFile, STGM_READWRITE);
					pIPersistFile->Release();
				}
				TCHAR szDesc[MAX_PATH] = {0};
				WIN32_FIND_DATA wfd = {0};
				pIShellLink->GetDescription(szDesc, _countof(szDesc));
				pIShellLink->Release();
				std::map<tstring, tstring>::iterator iter = LinkList.find(szDesc);
				tstring strFileName = PathFindFileName(szLinkFile);
				tstring strFileExt = PathFindExtension(szLinkFile);
				DWORD nOldLenth = (DWORD)(strFileName.length() - strFileExt.length());
				if (_tcsnicmp(szDesc, TEXT("sig="), 4) == 0 && 
					(iter== LinkList.end() || 
					_tcsnicmp(iter->second.c_str(),strFileName.c_str(),
					iter->second.length() > nOldLenth ? iter->second.length():nOldLenth) != 0))
				{
					SetFileAttributes(szLinkFile, FILE_ATTRIBUTE_NORMAL);
					DeleteFile(szLinkFile);
					bResult = TRUE;
					AfxWriteLogger(ghLogger, TEXT("delete desktop link:%s"), szLinkFile);
				}
			}
		} while (FindNextFile(hFinder, &wfd) == TRUE);
		FindClose(hFinder);
	}
	return bResult;
}

void CTaskServer::CreateLinkHelper()
{
	try
	{
		std::vector<tagGameShortCutItem> arTagGame;
		std::vector<tagSystemShortCutItem> arTagSystem;

		bool bRefresh = false;
		TCHAR szRootPath[MAX_PATH] = {0}, szIconFile[MAX_PATH] = {0};
		TCHAR szName[MAX_PATH] = {0}, szExePath[MAX_PATH] = {0}, szParam[MAX_PATH] = {0}, szIconPath[MAX_PATH] = {0}, szMemo[MAX_PATH] = {0};
		AfxGetAppPath(szRootPath);

		//create game desklnk.
		for (std::map<DWORD, tGameInfo>::iterator it = gGameInfo.begin(); it != gGameInfo.end(); it++)
		{
			if (!it->second.desklnk)
				continue ;
			tagGameShortCutItem tagItem = {0};
			tagItem.gId = it->first;
			DWORD gid = it->first;
			AfxGetAppPath(szExePath, _countof(szExePath), TEXT("knbmenu.exe"));
			_stprintf_s(szParam, _countof(szParam), TEXT("/gid=%d"), gid);
			_stprintf_s(szIconFile, _countof(szIconFile), TEXT("%s%s%d.ico"), szRootPath, IMAGE_DIR, gid);
			_stprintf_s(szMemo, _countof(szMemo), TEXT("sig=%s"), it->second.gname);

			_tcscpy_s(tagItem.gname,it->second.gname);
			_tcscpy_s(tagItem.szExePath,szExePath);
			_tcscpy_s(tagItem.szParam,szParam);
			_tcscpy_s(tagItem.szIconFile,szIconFile);
			_tcscpy_s(tagItem.szMemo,szMemo);
			arTagGame.push_back(tagItem);
			m_LinkList.insert(std::make_pair(szMemo, it->second.gname));

		}

		//create system desklnk.
		CString strUdisk, strScreenLock, strInputMethod, strMouseSet;
		TCHAR szTmp[MAX_PATH] = {0};
		strUdisk = AfxGetLangStringEx(ghModule, IDS_UDISK, szTmp, MAX_PATH);
		strScreenLock = AfxGetLangStringEx(ghModule, IDS_SCREEN_LOCK, szTmp, MAX_PATH);
		strInputMethod = AfxGetLangStringEx(ghModule, IDS_INPUT_METHOD, szTmp, MAX_PATH);
		strMouseSet = AfxGetLangStringEx(ghModule, IDS_MOUSESET, szTmp, MAX_PATH);

		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETSHORTCUT, OBJECT_CLIENT);
		pkgSend << gMachine;
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		DWORD dwSize = 0;
		pkgRecv >> dwSize;
		for (DWORD i=0; i<dwSize; ++i)
		{
			DWORD dwType = 0;
			pkgRecv >> dwType >> szName >> szExePath >> szParam >> szIconPath;
			LPTSTR szIconShortName = PathFindFileName(szIconPath);
			if (szIconFile != NULL)
				_stprintf_s(szIconFile, _countof(szIconFile), TEXT("%s%s%s"), szRootPath, IMAGE_DIR, szIconShortName);
			_stprintf_s(szMemo, _countof(szMemo), TEXT("sig=%s"), szName);

			switch (dwType)
			{
			case SHORTCUT_TYPE_GAME:
				{
					DWORD gid = _ttoi(szName);
					std::map<DWORD, tGameInfo>::iterator it = gGameInfo.find(gid);
					if (it != gGameInfo.end())
					{
						lstrcpy(szName, it->second.gname);
						AfxGetAppPath(szExePath, _countof(szExePath), TEXT("knbmenu.exe"));
						_stprintf_s(szParam, _countof(szParam), TEXT("/gid=%d"), gid);
						_stprintf_s(szIconFile, _countof(szIconFile), TEXT("%s%s%d.ico"), szRootPath, IMAGE_DIR, gid);

						tagSystemShortCutItem tagItem = {0};
						tagItem.dwType = dwType;
						tagItem.gId =  gid;
						_tcscpy_s(tagItem.szName,szName);
						_tcscpy_s(tagItem.szExePath,szExePath);
						_tcscpy_s(tagItem.szParam,szParam);
						_tcscpy_s(tagItem.szIconPath,szIconPath);
						_tcscpy_s(tagItem.szIconFile,szIconFile);
						_tcscpy_s(tagItem.szMemo,szMemo);
						arTagSystem.push_back(tagItem);
						m_LinkList.insert(std::make_pair(szMemo, szName));
					}
				}
				break;
			case SHORTCUT_TYPE_CLASS:
				{
					AfxGetAppPath(szExePath, _countof(szExePath), TEXT("knbmenu.exe"));
					_stprintf_s(szParam, _countof(szParam), TEXT("/class=%s"), szName);
					tagSystemShortCutItem tagItem = {0};
					tagItem.dwType = dwType;
					_tcscpy_s(tagItem.szName,szName);
					_tcscpy_s(tagItem.szExePath,szExePath);
					_tcscpy_s(tagItem.szParam,szParam);
					_tcscpy_s(tagItem.szIconPath,szIconPath);
					_tcscpy_s(tagItem.szIconFile,szIconFile);
					_tcscpy_s(tagItem.szMemo,szMemo);
					arTagSystem.push_back(tagItem);
					m_LinkList.insert(std::make_pair(szMemo, szName));
				}
				break;
			case SHORTCUT_TYPE_TOOL:
				{
					if (lstrcmpi(szName, strUdisk) == 0)
					{
						AfxGetAppPath(szExePath, _countof(szExePath), TEXT("knbmenu.exe"));
						lstrcpy(szParam, TEXT("/udisk"));
						tagSystemShortCutItem tagItem = {0};
						tagItem.dwType = dwType;
						_tcscpy_s(tagItem.szName,szName);
						_tcscpy_s(tagItem.szExePath,szExePath);
						_tcscpy_s(tagItem.szParam,szParam);
						_tcscpy_s(tagItem.szIconPath,szIconPath);
						_tcscpy_s(tagItem.szIconFile,szIconFile);
						_tcscpy_s(tagItem.szMemo,szMemo);
						arTagSystem.push_back(tagItem);
						m_LinkList.insert(std::make_pair(szMemo, szName));
					}
					else if (lstrcmpi(szName, strScreenLock) == 0)
					{
						ZeroMemory(szParam, sizeof(szParam));
						AfxGetAppPath(szExePath, _countof(szExePath), TEXT("knbmenu.exe"));
						_stprintf_s(szParam, _countof(szParam), TEXT("/gid=-6"));
						tagSystemShortCutItem tagItem = {0};
						tagItem.dwType = dwType;
						_tcscpy_s(tagItem.szName,szName);
						_tcscpy_s(tagItem.szExePath,szExePath);
						_tcscpy_s(tagItem.szParam,szParam);
						_tcscpy_s(tagItem.szIconPath,szIconPath);
						_tcscpy_s(tagItem.szIconFile,szIconFile);
						_tcscpy_s(tagItem.szMemo,szMemo);
						arTagSystem.push_back(tagItem);
						m_LinkList.insert(std::make_pair(szMemo, szName));
					}
					else if(lstrcmpi(szName,strMouseSet) == 0)
					{
						AfxGetAppPath(szExePath, _countof(szExePath), TEXT("knbmenu.exe"));
						lstrcpy(szParam, TEXT("/mouse"));
						tagSystemShortCutItem tagItem = {0};
						tagItem.dwType = dwType;
						_tcscpy_s(tagItem.szName,szName);
						_tcscpy_s(tagItem.szExePath,szExePath);
						_tcscpy_s(tagItem.szParam,szParam);
						_tcscpy_s(tagItem.szIconPath,szIconPath);
						_tcscpy_s(tagItem.szIconFile,szIconFile);
						_tcscpy_s(tagItem.szMemo,szMemo);
						arTagSystem.push_back(tagItem);
						m_LinkList.insert(std::make_pair(szMemo, szName));
					}
					else if (lstrcmpi(szName, strInputMethod) == 0)
					{
						AfxGetAppPath(szExePath, _countof(szExePath), TEXT("knbmenu.exe"));
						_stprintf_s(szParam, _countof(szParam), TEXT("/gid=42922"));
						tagSystemShortCutItem tagItem = {0};
						tagItem.dwType = dwType;
						_tcscpy_s(tagItem.szName,szName);
						_tcscpy_s(tagItem.szExePath,szExePath);
						_tcscpy_s(tagItem.szParam,szParam);
						_tcscpy_s(tagItem.szIconPath,szIconPath);
						_tcscpy_s(tagItem.szIconFile,szIconFile);
						_tcscpy_s(tagItem.szMemo,szMemo);
						arTagSystem.push_back(tagItem);
						m_LinkList.insert(std::make_pair(szMemo, szName));
					}
				}
				break;
			case SHORTCUT_TYPE_EXE:
			case SHORTCUT_TYPE_URL:
				{
					tagSystemShortCutItem tagItem = {0};
					tagItem.dwType = dwType;
					_tcscpy_s(tagItem.szName,szName);
					_tcscpy_s(tagItem.szExePath,szExePath);
					_tcscpy_s(tagItem.szParam,szParam);
					_tcscpy_s(tagItem.szIconPath,szIconPath);
					_tcscpy_s(tagItem.szIconFile,szIconFile);
					_tcscpy_s(tagItem.szMemo,szMemo);
					arTagSystem.push_back(tagItem);
					m_LinkList.insert(std::make_pair(szMemo, szName));
				}
				break;
			}
		}
		bRefresh |= DeleteMoreLink(m_LinkList);

		for (size_t i=0;i<arTagGame.size();i++)
		{
			if (DownGameDesktopIco(arTagGame[i].gId, arTagGame[i].szIconFile) || ExactIconFromDb(arTagGame[i].gId, arTagGame[i].szIconFile))
				CreateDesktopLink(arTagGame[i].gname, arTagGame[i].szExePath, arTagGame[i].szParam, arTagGame[i].szIconFile, arTagGame[i].szMemo, bRefresh);
			else
				AfxWriteLogger(ghLogger, TEXT("ExcatIcon %d fail."), arTagGame[i].gId);
		}

		for (size_t i=0;i<arTagSystem.size();i++)
		{
			switch (arTagSystem[i].dwType)
			{
			case SHORTCUT_TYPE_GAME:
				{
					if (DownGameDesktopIco(arTagSystem[i].gId, arTagSystem[i].szIconFile)|| ExactIconFromDb(arTagSystem[i].gId, arTagSystem[i].szIconFile))
						CreateDesktopLink(arTagSystem[i].szName, arTagSystem[i].szExePath, arTagSystem[i].szParam, arTagSystem[i].szIconFile, arTagSystem[i].szMemo, bRefresh);
					else
						AfxWriteLogger(ghLogger, TEXT("ExcatIcon %d fail."), arTagSystem[i].gId);
				}
				break;
			case SHORTCUT_TYPE_CLASS:
				{
					DownLoadFile(arTagSystem[i].szIconPath, arTagSystem[i].szIconFile, -1, -1, TRUE);
					CreateDesktopLink(arTagSystem[i].szName, arTagSystem[i].szExePath, arTagSystem[i].szParam, arTagSystem[i].szIconFile, arTagSystem[i].szMemo, bRefresh);
				}
				break;
			case SHORTCUT_TYPE_TOOL:
				{
					DownLoadFile(arTagSystem[i].szIconPath, arTagSystem[i].szIconFile, -1, -1, TRUE);
					CreateDesktopLink(arTagSystem[i].szName, arTagSystem[i].szExePath, arTagSystem[i].szParam, arTagSystem[i].szIconFile, arTagSystem[i].szMemo, bRefresh);
				}
				break;
			case SHORTCUT_TYPE_EXE:
			case SHORTCUT_TYPE_URL:
				{
					DownLoadFile(arTagSystem[i].szIconPath, arTagSystem[i].szIconFile, -1, -1, TRUE);
					CreateDesktopLink(arTagSystem[i].szName, arTagSystem[i].szExePath, arTagSystem[i].szParam, arTagSystem[i].szIconFile, arTagSystem[i].szMemo, bRefresh);
				}
				break;
			}
		}

		if (bRefresh)
			SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST|SHCNF_FLUSH, 0, 0);
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, TEXT("create desktop link fail.%d"),e);
	}
	catch (...)
	{
		AfxWriteLogger(ghLogger, TEXT("create desktop link fail."));
	}

	if (!AfxIsSuperUser())
		SaveDialogInfo();
}

void CTaskServer::SetWallPaper()
{
	if(m_bPushWallpaper)
		return;
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETWALLPAPER, OBJECT_CLIENT);
		pkgSend << gMachine;
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		DWORD dwSize = 0;
		pkgRecv >> dwSize;
		srand(_time32(NULL));
		int nIndex =  dwSize ? (rand() % dwSize) : 0;
		for (DWORD loop=0; loop<dwSize; ++loop)
		{
			TCHAR szFileName[MAX_PATH] = {0}, szImage[MAX_PATH] = {0};
			DWORD dwStyle = 0;
			pkgRecv >> szFileName >> dwStyle;
			if (nIndex == loop)
			{
				AfxGetAppPath(szImage, MAX_PATH, IMAGE_DIR);
				lstrcat(szImage, PathFindFileName(szFileName));
				if (!DownLoadFile(szFileName, szImage, -1, -1, FALSE))
				{
					AfxWriteLogger(ghLogger, TEXT("down wallpaper file fail:%s"), szImage);
					nIndex ++;
					continue;
				}

				TCHAR szExe[MAX_PATH] = {0}, szParameter[MAX_PATH] = {0};
				AfxGetAppPath(szExe, MAX_PATH, TEXT("kdesk.exe"));
				_stprintf_s(szParameter, _countof(szParameter), TEXT("\"/SetWallPaper=%s\""), szImage);
				AfxRunProcess(szExe, szParameter);
				AfxWriteLogger(ghLogger, TEXT("set wallpaper:%s"), szImage);
				break;
			}
		}
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, TEXT("set desktop wallpaper fail.%d"),e);
	}
	catch (...)
	{
		AfxWriteLogger(ghLogger, TEXT("set desktop wallpaper fail."));
	}
}

DWORD  AfxInstallDriver(LPCTSTR lpszSvrName,  LPCTSTR path,int StartType)
{
	TCHAR szExeName[MAX_PATH] = {0};
	if (path == NULL || lstrlen(path) == 0)
		GetModuleFileName(NULL, szExeName, _countof(szExeName));
	else
		lstrcpy(szExeName, path);
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT|SC_MANAGER_CREATE_SERVICE);
	if (schSCManager == NULL)
		return GetLastError();

	SC_HANDLE schService = CreateService(
		schSCManager,               // SCManager database
		lpszSvrName,				// name of service
		lpszSvrName,				// name to display
		SERVICE_ALL_ACCESS,         // desired access
		SERVICE_KERNEL_DRIVER,		// service type
		StartType,					// start type
		SERVICE_ERROR_NORMAL,       // error control type
		szExeName,					// service's binary
		NULL,                       // no load ordering group
		NULL,                       // no tag identifier
		NULL,						// dependencies
		NULL,                       // LocalSystem account
		NULL);                      // no password

	if (schService == NULL)
	{
		DWORD dwError = GetLastError();
		if (ERROR_SERVICE_EXISTS != dwError)
		{
			CloseServiceHandle(schSCManager);
			return dwError;
		}
	}

	if(schService != NULL)
		CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return ERROR_SUCCESS;
}

#define  NET_MOON_SVRNAME _T("knetmoon")
void CTaskServer::RunNetMoonClient()
{
	if(AfxIsSuperUser() || GetSysOptInteger(OPT_M_NETMOONENABLE, 0) == 0)
		return;
	DWORD nError = ERROR_SUCCESS;

	if(!AfxIsServiceExist(NET_MOON_SVRNAME))
	{
		//TCHAR szBuffer[MAX_PATH] = {0};
		//kcore::AfxGetAppPath(szBuffer,MAX_PATH,_T("knetmoon.sys"));
		TCHAR szSys[MAX_PATH] = {0};
		GetSystemDirectory(szSys, MAX_PATH);
		PathAddBackslash(szSys);
		if(AfxIs64BitOpertionSystem())
			lstrcat(szSys, TEXT("drivers\\knetmoon64.sys"));
		else
			lstrcat(szSys, TEXT("drivers\\knetmoon.sys"));
		nError = AfxInstallDriver(NET_MOON_SVRNAME,szSys,SERVICE_DEMAND_START);
		if(nError != ERROR_SUCCESS)
		{
			AfxWriteLogger(ghLogger, TEXT("install knetmoon service faild:%d"),nError);
			return;
		}
	}

	if(AfxGetServiceState(NET_MOON_SVRNAME) != SERVICE_RUNNING)
	{
		nError = AfxStartService(NET_MOON_SVRNAME);
		if(nError != ERROR_SUCCESS)
			AfxWriteLogger(ghLogger, TEXT("start knetmoon service faild:%d"),nError);
	}
}
void CTaskServer::RunExeHelper()
{
	if (GetSysOptInteger(OPT_M_SYNCTIME, 0) == 1)
	{
		char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(szSendBuffer, CMD_CLI_GETSERVERTIME, OBJECT_CLIENT);
		if (ERROR_SUCCESS == AfxSocketExecuteLock(gSocket, szSendBuffer, pkgSend.GetLength(), szRecvBuffer, sizeof(szRecvBuffer)))
		{
			CPkgHelper  pkgRecv(szRecvBuffer);
			if (pkgRecv.IsStatusOk())
			{
				SYSTEMTIME sysTime = {0};
				pkgRecv >> sysTime;
				SetSystemTime(&sysTime);
				AfxWriteLogger(ghLogger, TEXT("Sync server time success."));
			}
		}
	}
	/*
	if (GetSysOptInteger(OPT_M_RUNMENU) == 1)
	{
		AfxRunProcess(TEXT("knbmenu.exe"));
		AfxWriteLogger(ghLogger, TEXT("start client game menu."));
	}
	
	if (GetSysOptInteger(OPT_M_SAFECENTER) == 1)
	{
		TCHAR szServerIp[MAX_PATH] = {0};
		AfxGetSysString(TEXT("config"), TEXT("serverip"), szServerIp, _countof(szServerIp), TEXT("127.0.0.1"));
		AfxRunProcess(TEXT("Tools\\KHSafe\\KHLauncher.exe"), szServerIp);
		AfxWriteLogger(ghLogger, TEXT("start client safte center."));
	}
	*/
	if ((GetSysOptInteger(OPT_M_IEHISTORYENABLE) == 1 || 
		GetSysOptInteger(OPT_M_QQHISTORYENABLE) == 1) && 
		!AfxIsSuperUser())
	{
		// 启动IE历史、QQ登录历史上报程序

		/*try
		{
		typedef HRESULT (*PFNDLLREGISTERSERVER)(BOOL RegTypeLib);
		TCHAR szDllFileName[MAX_PATH] = {0};
		AfxGetAppPath(szDllFileName, MAX_PATH, TEXT("IEHelper.dll"));
		HMODULE hModule = LoadLibrary(szDllFileName);
		PFNDLLREGISTERSERVER pfnDllRegisterServer = (PFNDLLREGISTERSERVER)GetProcAddress(hModule, "DllRegisterServer");

		TCHAR szServerIp[MAX_PATH] = {0};
		AfxGetSysString(TEXT("config"), TEXT("serverip"), szServerIp, _countof(szServerIp), TEXT("127.0.0.1"));
		SHSetValue(HKEY_LOCAL_MACHINE, SOFTWARE_KEY, TEXT("ServerIp"), REG_SZ, szServerIp, (lstrlen(szServerIp)+1) * sizeof(TCHAR));
		if (pfnDllRegisterServer != NULL)
		{
		pfnDllRegisterServer(TRUE);
		AfxWriteLogger(ghLogger, TEXT("start iehistory tool."));
		FreeLibrary(hModule);
		}
		}catch (...) {}*/

		TCHAR szServerIp[MAX_PATH] = {0};
		AfxGetSysString(TEXT("config"), TEXT("serverip"), szServerIp, _countof(szServerIp), TEXT("127.0.0.1"));
		DWORD nIp = inet_addr(_bstr_t(szServerIp));
		TCHAR strRoot[MAX_PATH] = {0};
		AfxGetAppPath(strRoot, MAX_PATH, IEHIST_PROCESS_NAME);
		if(PathFileExists(strRoot))
		{
			TCHAR szBufferIp[MAX_PATH] = {0};
			_stprintf_s(szBufferIp,_T("%d"),nIp);
			AfxRunProcess(strRoot,szBufferIp);
			AfxWriteLogger(ghLogger, TEXT("start iehistory tool."));
		}
		else
			AfxWriteLogger(ghLogger, TEXT("can't find iehistory tool."));


	}
	CreateLinkHelper();

	if(GetSysOptInteger(OPT_S_CLOUDDESKTOP,0) == 1)
	{
		//去掉桌面美化功能全由panda提供这些功能（壁纸除外）
		// 2017-1-4上线后又发觉这东西又有用。又把它恢复

		TCHAR szExe[MAX_PATH] = {0};
		AfxGetAppPath(szExe, MAX_PATH, TEXT("kdesk.exe"));
		AfxRunProcess(szExe);
		AfxWriteLogger(ghLogger, TEXT("start kdesk"));
		
	}

	//if (!AfxIsSuperUser() && GetSysOptInteger(OPT_S_CLOUDDESKTOP, 0) == 1)
	if (!AfxIsSuperUser())
		SetWallPaper();

	DWORD bIsAdmin = 0;
	ScsiDiskQueryDisklessInfo(NULL, &bIsAdmin, NULL);
	if (bIsAdmin)
		AfxRunProcess(TEXT("SuperStation.exe"),_T("/super=1"));
}

bool CTaskServer::ExecuteSingleBootTask(std::vector<tBootTask>::iterator& it)
{
	tBootTask& Task = *it;
	DWORD now = GetTickCount();
	switch (Task.TaskType)
	{
	case BOOT_TASK_RUNCLI:
		{
			if (PathFileExists(Task.ExeFile))
			{
				AfxRunProcess(Task.ExeFile, Task.Param);
				AfxWriteLogger(ghLogger, TEXT("run task(client):%s"), Task.ExeFile);
				return true;

			}
			else
			{
				if ((now - Task.lastExTime) >= 5000)
				{
					Task.lastExTime = now;
					AfxWriteLogger(ghLogger, TEXT("task(client):%s is not exist"), Task.ExeFile);
				}
				

			}
		}
		break;
	case BOOT_TASK_DOWNRUN:
		try
		{
			AfxWriteLogger(ghLogger, TEXT("start run task(server):%s"), Task.SvrDir);
			TCHAR szRootPath[MAX_PATH] = {0};
			AfxGetAppPath(szRootPath, MAX_PATH, TEXT("task\\"));
			lstrcat(szRootPath, Task.TaskId);
			PathAddBackslash(szRootPath);

			char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
			CPkgHelper pkgSend(SendBuffer, CMD_CLI_QUERYDIRINFO, OBJECT_CLIENT);
			pkgSend << Task.SvrDir;
			if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			{
				AfxWriteLogger(ghLogger, TEXT("query task disk fail."));
				throw (int)'e';
			}
			CPkgHelper pkgRecv(RecvBuffer);
			if (!pkgRecv.IsStatusOk())
			{
				TCHAR szErrInfo[MAX_PATH] = {0};
				pkgRecv >> szErrInfo;
				AfxWriteLogger(ghLogger, TEXT("Recv Task File List:%s:%s"), Task.SvrDir, szErrInfo);
				throw (int)'w';
			}

			DWORD dwCount = 0;
			pkgRecv >> dwCount;
			for (DWORD i=0; i<dwCount; ++i)
			{
				TCHAR szFileName[MAX_PATH] = {0}, szFullName[MAX_PATH] = {0};
				DWORD dwFileSize = 0, dwModifyTime = 0;
				pkgRecv >> szFileName >> dwFileSize >> dwModifyTime;
				_stprintf_s(szFullName, TEXT("%s%s"), szRootPath, &szFileName[lstrlen(Task.SvrDir)]);
				if (!DownLoadFile(szFileName, szFullName, dwFileSize, dwModifyTime, FALSE))
					AfxWriteLogger(ghLogger, TEXT("download task file fail:%s"), szFullName);
			}
			AfxWriteLogger(ghLogger, TEXT("run task(server):%s:%s%s"), Task.SvrDir, szRootPath, Task.ExeFile);
			if (lstrlen(Task.ExeFile))
			{
				lstrcat(szRootPath, Task.ExeFile);
				if (PathFileExists(szRootPath))
				{
					AfxRunProcess(szRootPath, Task.Param);
					return true;
				}
				else
				{
					if ((now - Task.lastExTime) >= 5000)
					{
						Task.lastExTime = now;
						AfxWriteLogger(ghLogger, TEXT("task(server):%s is not exist"), Task.ExeFile);
					}
				

				}
			}
			else
			{
				AfxWriteLogger(ghLogger, TEXT("task(server):%s:exe file is null"), Task.SvrDir);
			}
		}
		catch (int &e)
		{
			AfxWriteLogger(ghLogger, TEXT("ExecuteSingleBootTask fail %d"), e);
		}
		catch (...) {}
		break;
	}
	return false;
}

tstring CTaskServer::TranslateImagePath(const tstring& source)
{
	static const tstring sys0(TEXT("\\??\\"));
	static const tstring sys1(TEXT("\\SYSTEMROOT\\SYSTEM32\\"));
	static const tstring sys2(TEXT("SYSTEM32\\"));
	static tstring syspath;
	if (syspath.size() == 0)
	{
		TCHAR path[MAX_PATH] = {0};
		GetSystemDirectory(path, _countof(path)-1);
		PathAddBackslash(path);
		syspath = path;
	}

	tstring dest(source);
	if (_tcsnicmp(dest.c_str(), sys0.c_str(), sys0.size()) == 0)
		dest.erase(0, sys0.size());

	if (_tcsnicmp(dest.c_str(), sys1.c_str(), sys1.size()) == 0)
		dest.replace(0, sys1.size(), syspath);

	if (_tcsnicmp(dest.c_str(), sys2.c_str(), sys2.size()) == 0)
		dest.replace(0, sys2.size(), syspath);

	return dest;
}
tstring DosPathToNtPath(LPTSTR pszDosPath)
{
	TCHAR szNtPath[MAX_PATH] = {0};

	TCHAR			szDriveStr[500] = {0};
	INT				i;

	//检查参数
	if(!pszDosPath)
		return _T("");

	//获取本地磁盘字符串
	if(GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for(i = 0; szDriveStr[i]; i += 4)
		{
			if(!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
				continue;

			TCHAR			szDrive[3] = {0};
			TCHAR			szDevName[100] = {0};
			INT				cchDevName = 0;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			if(!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名
				return _T("");

			cchDevName = lstrlen(szDevName);
			if(_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//命中
			{
				lstrcpy(szNtPath, szDrive);//复制驱动器
				lstrcat(szNtPath, pszDosPath + cchDevName);//复制路径

				return tstring(szNtPath);
			}			
		}
	}
	return  _T("");
}

void CTaskServer::BaseDriverInfo()
{
	
	//c:\windows\system32
	wchar_t sysDir[260];
	GetSystemDirectory(sysDir, 260);
	std::vector<DWORD> splitepage;
	splitepage.push_back(sizeof(PkgHeader));
	std::vector<USHORT> countv;
	USHORT pagecount = 0;
	

	char SendBuffer[PKG_MAXLENGTH*4] = { 0 }, RecvBuffer[PKG_MAXLENGTH] = { 0 };
	CPkgHelper pkg(SendBuffer, CMD_CLI_CLIENT2CONSOLE, OBJECT_CLIENT);
	
	/*
	pkg << CONTROL_CLINET_DRIVERINFO;
	DWORD tick = GetTickCount();
	pkg << UINT(tick);
	pkg << UINT(1);//总包数
	pkg << UINT(1);//当前第几个包
	pkg << UINT(0);//条数
	*/

	//UINT total = 0;
	wchar_t * strPath = TEXT("SYSTEM\\CurrentControlSet\\Services");

	CRegKey regkey;
	CRegKey regkeysub;
	bool nc1 = false;
	if (ERROR_SUCCESS == regkey.Open(HKEY_LOCAL_MACHINE, strPath, KEY_ALL_ACCESS))
	{
		nc1 = true;
		UINT dwIndex = 0;			
		while (true)
		{
			TCHAR szExtension[MAX_PATH] = { 0 };		
			DWORD dwSize = MAX_PATH;
			TCHAR temp[MAX_PATH] = { 0 };
			int ret = regkey.EnumKey(dwIndex, szExtension, &dwSize);			
			dwIndex++;		
			if (ret == ERROR_NO_MORE_ITEMS)
				break;
			if (ret != ERROR_SUCCESS)
				continue;		
			if (wcslen(szExtension) < 0)
				continue;
			wcscpy_s(temp, strPath);
			wcscat_s(temp, TEXT("\\"));
			wcscat_s(temp, szExtension);
			wchar_t value[MAX_PATH] = { 0 };
			ULONG size = MAX_PATH;
			if (ERROR_SUCCESS == regkeysub.Open(HKEY_LOCAL_MACHINE, temp, KEY_ALL_ACCESS))
			{
				
				if (ERROR_SUCCESS == regkeysub.QueryStringValue(TEXT("ImagePath"), value, &size))
				{
					UINT point = wcslen(value);
					if (point < 5)
					{
						regkeysub.Close();
						continue;
					}
					if (wmemcmp(value, L"\\??\\", 4) == 0){					
						memmove(value , value+4, point - 4);
						point = wcslen(value);
					}
					point -= 4;
					std::wstring str(value);
					if (wmemcmp(&value[point], TEXT(".sys"), 4) == 0)
					{
						pkg << szExtension;	
						//total++;
						pagecount++;

						size_t found = str.find(TEXT("ystem32\\"),0);
						if (found != std::wstring::npos)
						{
							str.replace(0, found + wcslen(TEXT("ystem32")), sysDir);
							pkg << str.c_str();
						}
						else
							pkg << value;
					}
					else{
						regkeysub.Close();
						continue;
					}					
					DWORD dwSize = GetFileVersionInfoSize(str.c_str(), NULL);
					if (dwSize == 0)
					{
						pkg << TEXT("--");
						pkg << L"--";
						regkeysub.Close();
						continue;
					}

					LPTSTR pblock = new TCHAR[dwSize + 1];
					GetFileVersionInfo(str.c_str(), 0, dwSize, pblock);
					UINT nQuerySize;
					DWORD* pTransTable = NULL;
					VerQueryValue(pblock, TEXT("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize);
					LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
					TCHAR SubBlock[MAX_PATH] = { 0 };
					_stprintf_s(SubBlock, MAX_PATH, TEXT("\\StringFileInfo\\%08lx\\CompanyName"), m_dwLangCharset);
					LPTSTR lpData;
					if (VerQueryValue(pblock, SubBlock, (PVOID*)&lpData, &nQuerySize))
						pkg << lpData;
					else
						pkg << TEXT("--");
					delete[] pblock;	
					
					wchar_t md5[33] = { 0 };					
					formatfilepath((wchar_t*)str.c_str());
					//PVOID OldValue;
					//Wow64DisableWow64FsRedirection(&OldValue);
					EnabelWow64FsRedirection(FALSE);
					AfxGetFileMd5(str.c_str(), md5);
					//Wow64RevertWow64FsRedirection(OldValue);	
					if (GetLastError() == 0)
						pkg << md5;
					else
					{
						pkg << L"--";
						AfxWriteLogger(ghLogger, TEXT("get %s md5 fail errno:%d"), str.c_str(),GetLastError());					
					}
					EnabelWow64FsRedirection(TRUE);
					{
						DWORD  posit = pkg.GetOffset();
						if (posit - splitepage[splitepage.size() - 1] >= (0x10000 /4 * 3))
						{
							splitepage.push_back(posit);
							countv.push_back(pagecount);
							pagecount = 0;
						}
					}


				}	
				regkeysub.Close();
			}
			
		}

	}	
	if (nc1)
		regkey.Close();

	DWORD  posit = pkg.GetOffset();
	splitepage.push_back(posit);
	countv.push_back(pagecount);

	/*
	if (pkg.GetLength() <= PKG_MAXLENGTH)
	{
		*(UINT*)(pkg.GetHeader()->data + 4 * 3) = total;
		AfxSocketExecuteLock(gSocket, SendBuffer, pkg.GetLength(), RecvBuffer, PKG_MAXLENGTH);
		return;
	}
	else
	*/
	{
		DWORD tick = GetTickCount();	
		UINT totalpack = splitepage.size() - 1; //0   1  2    2
		for (UINT i = 0; i < totalpack; i++)
		{
			char SendPack[PKG_MAXLENGTH] = { 0 };
			CPkgHelper pkgson(SendPack, CMD_CLI_CLIENT2CONSOLE, OBJECT_CLIENT);
			pkgson << CONTROL_CLINET_DRIVERINFO;			
			pkgson << UINT(tick);
			pkgson << UINT(totalpack);//总包数
			pkgson << UINT(i+1);//当前第几个包
			pkgson << UINT(countv[i]);//条数
			UINT len = splitepage[i + 1] - splitepage[i];
			char *p = (char*)pkg.GetBuffer() /*+ sizeof(PkgHeader)*/ + splitepage[i];
			memcpy((char*)pkgson.GetBuffer() + pkgson.GetOffset(), p,len );
			pkgson.SetLength(pkgson.GetLength() +len);
			AfxSocketExecuteLock(gSocket, SendPack, pkgson.GetLength(), RecvBuffer, PKG_MAXLENGTH);
		}
	}


}
bool CTaskServer::GetExpertMemory(HANDLE proid, size_t& value)
{
	if (pagesize == -1)
		return false;
	value = 0;
	BOOL bRet = TRUE;
	PSAPI_WORKING_SET_INFORMATION workSetInfo;
	PBYTE pByte = NULL;
	PSAPI_WORKING_SET_BLOCK *pWorkSetBlock = workSetInfo.WorkingSetInfo;
	memset(&workSetInfo, 0, sizeof(workSetInfo));
	bRet = ::QueryWorkingSet(proid, &workSetInfo, sizeof(workSetInfo));
	if (!bRet)
	{
		if (GetLastError() == ERROR_BAD_LENGTH)
		{
			DWORD realSize = sizeof(workSetInfo.NumberOfEntries)
				+ workSetInfo.NumberOfEntries*sizeof(PSAPI_WORKING_SET_BLOCK);
			pByte = new BYTE[realSize];
			memset(pByte, 0, realSize);
			pWorkSetBlock = (PSAPI_WORKING_SET_BLOCK *)(pByte + sizeof(workSetInfo.NumberOfEntries));
			if (!::QueryWorkingSet(proid, pByte, realSize))
			{
				AfxWriteLogger(ghLogger, TEXT("QueryWorkingSet last error=:%d"), GetLastError());
				delete[] pByte;
				return false;

			}
		}
		
	}
		
	for (ULONG_PTR i = 0; i < workSetInfo.NumberOfEntries; ++i)
	{
		if (!pWorkSetBlock[i].Shared)
			value += pagesize;
	}
	if (pByte)
		delete[] pByte;
	return true;	
}
void CTaskServer::BaseInfoQuery()
{
	char SendBuffer[PKG_MAXLENGTH] = { 0 };
	char RecvBuffer[PKG_MAXLENGTH] = { 0 };

	CPkgHelper pkg(SendBuffer, CMD_CLI_CLIENT2CONSOLE, OBJECT_CLIENT);
	pkg << CONTROL_CLINET_BASEINFO;
	pkg << gMachine;//pc-name;	
	OS_VER osVer = OS_UNKNOWN;
	RTL_OSVERSIONINFOEXW info;
	AfxGetOSVerEx(osVer, info);
	bool is64=IsWow64();
	wchar_t vername[MAX_PATH] = { 0 };
	switch (osVer)
	{	
	case	OS_WIN95:
		wcscpy_s(vername, MAX_PATH, TEXT("windows 95"));
		break;
	case	OS_WIN98:		
		wcscpy_s(vername, MAX_PATH, TEXT("windows 98")); 
		break;
	case	OS_WINME:
		wcscpy_s(vername, MAX_PATH, TEXT("windows me"));
		break;
	case	OS_NT351:
		wcscpy_s(vername, MAX_PATH, TEXT("windows nt351"));
		break;
	case	OS_NT4:
		wcscpy_s(vername, MAX_PATH, TEXT("windows nt4"));
		break; 
	case	OS_WIN2000:
		wcscpy_s(vername, MAX_PATH, TEXT("windows 2000"));
		break;
	case	OS_WINXP:
		wcscpy_s(vername, MAX_PATH, TEXT("windows xp"));
		break;
	case	OS_VISTA:
		wcscpy_s(vername, MAX_PATH, TEXT("windows vista"));
		break;
	case	OS_WIN7:
		wcscpy_s(vername, MAX_PATH, TEXT("windows 7"));
		break;
	case	OS_WIN8:
		wcscpy_s(vername, MAX_PATH, TEXT("windows 8"));
		break;
	case	OS_WIN10:
		wcscpy_s(vername, MAX_PATH, TEXT("windows 10"));
		break;
	case	OS_NEWEST:
		wcscpy_s(vername, MAX_PATH, TEXT("windows newest"));
		break;
	case	OS_UNKNOWN:
		wcscpy_s(vername, MAX_PATH, TEXT("unknown"));
		break;
	default:
		wcscpy_s(vername, MAX_PATH, TEXT("unknown"));
		break;
	}
	wcscat_s(vername, MAX_PATH - wcslen(vername), info.szCSDVersion);
	wcscat_s(vername, MAX_PATH - wcslen(vername), is64 ? TEXT(" 64 bit") : TEXT(" 32 bit"));
	pkg << vername;
	memset(vername, 0, sizeof(vername));		
	AfxGetFileVersion(TEXT("knbclient.exe"), vername);
	pkg << vername;

	UINT starttime = GetTickCount();
	UINT miu_tot = starttime / 1000 / 60;
	UINT day = miu_tot / 60 / 24;
	UINT hour = miu_tot / 60 % 24;
	UINT min = miu_tot % 60;
	memset(vername, 0x0, sizeof(vername));
	swprintf_s(vername, MAX_PATH,TEXT("%d days %d hours %d minutes"), day,hour,min);
	pkg << vername;


	CString strPath = _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"); 
	CRegKey regkey;
	LONG lResult;
	lResult = regkey.Open(HKEY_LOCAL_MACHINE, LPCTSTR(strPath), KEY_ALL_ACCESS); 
	bool getcpuinfo = false;
	if (lResult == ERROR_SUCCESS)
	{
		wchar_t chCPUName[50] = { 0 };
		DWORD dwSize = 50;
		if (ERROR_SUCCESS == regkey.QueryStringValue(_T("ProcessorNameString"), chCPUName, &dwSize))
			pkg << chCPUName;

		else
			pkg << TEXT("err0");
		regkey.Close();
		
	}else
		pkg << TEXT("err1");
	MEMORYSTATUSEX lpBuffer = { 0 };
	lpBuffer.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&lpBuffer);
	memset(vername, 0x0, sizeof(vername));
	swprintf_s(vername,MAX_PATH, TEXT("%.2fMB"), (float)lpBuffer.ullTotalPhys/1024/1024);
	pkg << vername;

	memset(vername, 0x0, sizeof(vername));
	swprintf_s(vername, MAX_PATH, TEXT("%.2fMB"), (float)lpBuffer.ullAvailPhys / 1024 / 1024);
	pkg << vername;

	memset(vername, 0x0, sizeof(vername));
	swprintf_s(vername, MAX_PATH, TEXT("%.2fMB"), (float)lpBuffer.ullTotalVirtual / 1024 / 1024);
	pkg << vername;


	memset(vername, 0x0, sizeof(vername));
	swprintf_s(vername, MAX_PATH, TEXT("%.2fMB"), (float)lpBuffer.ullAvailVirtual / 1024 / 1024);
	pkg << vername;

	AfxSocketExecuteLock(gSocket, SendBuffer, pkg.GetLength(), RecvBuffer, PKG_MAXLENGTH);
}
void CTaskServer::ViewProcessList()
{	
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	
	struct timeobj
	{
		FILETIME CreateTime;
		FILETIME ExitTime;
		FILETIME KernelTime;
		FILETIME UserTime;
	};
	std::map<DWORD, timeobj> pro_list;
	
	ULONGLONG pstart=GetTickCount();
	
	HANDLE hProcessSnap = NULL; 
	try
	{
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			throw GetLastError();

		PROCESSENTRY32 pe32 = { 0 };
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(hProcessSnap, &pe32))
			throw GetLastError();
	
		do
		{		

			TCHAR szProcessName[MAX_PATH] = { 0 };
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
			if (hProcess != NULL)
			{	
				timeobj obj = { 0 };
				if(GetProcessTimes(hProcess, &obj.CreateTime, &obj.ExitTime, &obj.KernelTime, &obj.UserTime))
					pro_list.insert(std::pair < DWORD, timeobj >(pe32.th32ProcessID, obj));
				CloseHandle(hProcess);
			}
			
		} while (Process32Next(hProcessSnap, &pe32));
	
	}
	catch (DWORD &e) 
	{
		AfxWriteLogger(ghLogger, TEXT("ViewProcessList fail:%d"), e);
	}
	catch (...) {};
	if (hProcessSnap != NULL)
		CloseHandle(hProcessSnap);

	
	if (WaitForSingleObject(ghExited, 1000) != WAIT_TIMEOUT)
		return;


	try
	{
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			throw GetLastError();

		PROCESSENTRY32 pe32 = {0};
		pe32.dwSize = sizeof( PROCESSENTRY32 );
		if (!Process32First( hProcessSnap, &pe32))
			throw GetLastError();

		//generate process list.
		MSXML2::IXMLDOMDocumentPtr xmlDoc(__uuidof(MSXML2::DOMDocument));
		MSXML2::IXMLDOMElementPtr  doc = xmlDoc->createElement(TEXT("pl"));
		do
		{
			MSXML2::IXMLDOMElementPtr Node = xmlDoc->createElement(TEXT("p"));
			Node->setAttribute(TEXT("e"), pe32.szExeFile);
			Node->setAttribute(TEXT("i"), pe32.th32ProcessID);

			TCHAR szProcessName[MAX_PATH] = {0};
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
			if (hProcess != NULL)
			{
				/*GetModuleFileNameEx(hProcess, NULL, szProcessName, _countof(szProcessName));
				tstring path = TranslateImagePath(szProcessName);*/
				GetProcessImageFileName (hProcess, szProcessName, _countof(szProcessName));
				tstring path = DosPathToNtPath(szProcessName);
				Node->setAttribute(TEXT("p"), _bstr_t(path.c_str()));

				wchar_t md5[33] = { 0 };
				formatfilepath((wchar_t*)path.c_str());
			
				//PVOID OldValue;
				//Wow64DisableWow64FsRedirection(&OldValue);
				EnabelWow64FsRedirection(FALSE);
				AfxGetFileMd5(path.c_str(), md5);
				//Wow64RevertWow64FsRedirection(OldValue);					
				if (GetLastError() == 0)
					Node->setAttribute(TEXT("k"), _bstr_t(md5));
				else
				{
					Node->setAttribute(TEXT("k"),"--");				
					AfxWriteLogger(ghLogger, TEXT("get %s md5 fail errno:%d"), path.c_str(), GetLastError());
				}
				EnabelWow64FsRedirection(TRUE);

				size_t value = 0;
				if(GetExpertMemory(hProcess, value))				
						Node->setAttribute(TEXT("m"), (DWORD)((value) / 1024));
				else{
					PROCESS_MEMORY_COUNTERS_EX pmc = { 0 };
					pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);
					if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
						Node->setAttribute(TEXT("m"), (DWORD)((pmc.WorkingSetSize) / 1024));
				}				
				FILETIME	CreateTime, ExitTime, KernelTime, UserTime;
				if (!GetProcessTimes(hProcess, &CreateTime, &ExitTime, &KernelTime, &UserTime))
				{
					Node->setAttribute(TEXT("d"), _bstr_t(TEXT("--")));
					CloseHandle(hProcess);
					continue;
				}
				auto it=pro_list.find(pe32.th32ProcessID);
				if (it == pro_list.end())
				{
					Node->setAttribute(TEXT("d"), _bstr_t(TEXT("--")));
					CloseHandle(hProcess);
					continue;
				}
				
				LARGE_INTEGER t1, t2;
				t1.QuadPart = t2.QuadPart = 0;

				t1.HighPart = it->second.KernelTime.dwHighDateTime + it->second.UserTime.dwHighDateTime;
				t1.LowPart = it->second.KernelTime.dwLowDateTime + it->second.UserTime.dwLowDateTime;

				t2.HighPart = KernelTime.dwHighDateTime + UserTime.dwHighDateTime;
				t2.LowPart = KernelTime.dwLowDateTime+UserTime.dwLowDateTime;

				ULONGLONG pstop = GetTickCount();
				int per=(int)((t2.QuadPart - t1.QuadPart)/ (pstop-pstart) / info.dwNumberOfProcessors/100);
				wchar_t perper[16] = { 0 };
				swprintf_s(perper, 16,TEXT("%d"), per);
				Node->setAttribute(TEXT("d"), perper);
				CloseHandle(hProcess);
			}
			doc->appendChild(Node);
		}while (Process32Next(hProcessSnap, &pe32));

		//send info.
		char SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		tstring xml(doc->xml);
		CPkgHelper pkg(SendBuffer, CMD_CLI_CLIENT2CONSOLE, OBJECT_CLIENT);
		pkg << CONTROL_CLINET_VIEWPROCLIST << xml;
		AfxSocketExecuteLock(gSocket, SendBuffer, pkg.GetLength(), RecvBuffer, PKG_MAXLENGTH);
	}
	catch (DWORD &e) 
	{
		AfxWriteLogger(ghLogger, TEXT("CreateToolhelp32Snapshot fail:%d"), e);
	}
	catch (...) { };
	if (hProcessSnap != NULL)
		CloseHandle(hProcessSnap);
}

bool WINAPI RapidShut(int reboot)
{
	typedef enum
	{
		ShutdownNoReboot,
		ShutdownReboot,
		ShutdownPowerOff,
	}SHUTDOWN_ACTION;

	typedef DWORD (WINAPI *PFNNTSHUTDOWNSYSTEM)(SHUTDOWN_ACTION action);
	typedef int (WINAPI *PFNADJUSTPRIVILEGES)(int,int,int,int *);

	HMODULE hMod = LoadLibrary(TEXT("ntdll.dll"));
	if (!hMod)
		return false;

	PFNADJUSTPRIVILEGES pfnAdjustPrivileges =(PFNADJUSTPRIVILEGES)GetProcAddress(hMod, "RtlAdjustPrivilege");
	if (pfnAdjustPrivileges)
	{
		int dwOldVal = NULL;
		pfnAdjustPrivileges(19, TRUE, FALSE, &dwOldVal);
		PFNNTSHUTDOWNSYSTEM pfnShutdownSystem = (PFNNTSHUTDOWNSYSTEM)GetProcAddress(hMod, "NtShutdownSystem");
		if (pfnShutdownSystem)
		{
			pfnShutdownSystem(reboot ? ShutdownReboot : ShutdownPowerOff);
			return true;
		}
	}

	return false;
}

void CTaskServer::UdpRecvNotify(LPCSTR pData, DWORD length, sockaddr_in& from)
{
	char buf[PKG_MAXLENGTH] = {0};
	CPkgHelper  pkgRecv(pData);
	DWORD cmd = 0;
	pkgRecv >> cmd;
	switch (cmd)
	{
	case CONTROL_CLINET_INSTPROT:
		{
			TCHAR szProt[MAX_PATH] = {0}, szUnProt[MAX_PATH] = {0};
			pkgRecv >> szProt >> szUnProt;
			AfxWriteLogger(ghLogger, TEXT("Recv Prot Install, Prot Area:[%s]:UnProt:[%s]."), szProt, szUnProt);
			if (!hy_IsInstall())
			{
				hy_Install(szProt);
				SHSetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager"), TEXT("BootExecute"), REG_MULTI_SZ, "", 0);
			}
			else
			{
				hy_PortArea(szProt, false);
				hy_UnPortArea(szUnProt, false);
				AfxControlMachine(true);
			}
		}
		break;
	case CONTROL_CLINET_REMOVEPROT:
		{
			bool ret = hy_Uninstall();
			AfxWriteLogger(ghLogger, TEXT("Recv Remove Prot:%d"), ret ? 1 : 0);
		}
		break;
	case CONTROL_CLINET_SHUTDOWN:
		{
			DWORD nReboot = 0;
			bool bReboot = false;
			pkgRecv >> nReboot;
			bReboot = (nReboot != 0);
			AfxWriteLogger(ghLogger, bReboot ? TEXT("Recv ReBoot Machine.") : TEXT("Recv ShutDown Machine."));
			AfxControlMachine(bReboot);
		}
		break;
	case CONTROL_CLIENT_RAPIDSHUTDOWN:	//for win10 fast shutdown, send from hooked exitwindowsex in winlogon.exe
		{
			DWORD nReboot = 0;
			bool bReboot = false;
			pkgRecv >> nReboot;
			bReboot = (nReboot != 0);
			AfxWriteLogger(ghLogger, bReboot ? TEXT("Recv Fast ReBoot Machine.") : TEXT("Recv Fast ShutDown Machine."));
			//if (from == "127.0.0.1")		//for safety, check ip is from local host
			RapidShut(bReboot);
		}
		break;
	case CONTROL_CLINET_VIEWPROCLIST:
		{
			ViewProcessList();
			AfxWriteLogger(ghLogger, TEXT("Recv View Process List."));
		}
		break;
	case CONTROL_CLINET_KILLPROC:
		{
			DWORD pid = 0;
			pkgRecv >> pid;
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}
			AfxWriteLogger(ghLogger, TEXT("Recv Kill Process:%u"), pid);
		}
		break;
	case CONTROL_CLIENT_RUNPROCESS:
		{
			TCHAR szProcess[MAX_PATH] = {0};
			TCHAR szParam[MAX_PATH] = {0};
			tstring strMac = gMac;
			TCHAR szKey[MAX_PATH] = {0};
			tstring strRight = strMac.substr(strMac.length()-8,8);
			_tcscpy_s(szKey,strRight.c_str());
			TCHAR szCltFile[MAX_PATH] = {0};
			TCHAR szParamter[MAX_PATH] = {0};
			int nLenthPocess = 0;
			int nLenthParam = 0;
			nLenthPocess = pkgRecv.PopString(szProcess);
			nLenthParam = pkgRecv.PopString(szParam);
			AfxDecryTea(szProcess,nLenthPocess,szKey,(int)_tcslen(szKey),szCltFile,MAX_PATH);
			AfxDecryTea(szParam,(int)_tcslen(szParam),szKey,(int)_tcslen(szKey),szParamter,MAX_PATH);
			AfxRunProcess(szCltFile, szParamter);
			AfxWriteLogger(ghLogger, TEXT("Recv Run Process:%s."), szCltFile);
		}
		break;
	case CONTROL_CLIENT_RUNVNC:
		{
			//start vnc
			TCHAR szVncPath[MAX_PATH] = {0};
			AfxGetAppPath(szVncPath,MAX_PATH,_T("tools\\kvnc\\kvnserver.exe"));
			//unstall vnc service
			if(AfxIsServiceExist(_T("kvnserver")))
				AfxRunProcess(szVncPath,_T("-remove -silent"));
			if (AfxGetProcIdByName(VNC_SERVIECE_NAME) == 0)
			{
				AfxRunProcess(szVncPath,_T(""));
				AfxWriteLogger(ghLogger, TEXT("start vnc exe."));
			}
		}
		break;

	case CONTROL_CLINET_BASEINFO:
	{
		BaseInfoQuery();
		AfxWriteLogger(ghLogger, TEXT("Recv baseinfoquery List."));
	}
	break;
	case CONTROL_CLINET_PING:
		{
		wchar_t cmdline[MAX_PATH] = { 0 };
		 UINT start = 0;
		pkgRecv >> start;
		pkgRecv >> cmdline;
		if (!start)
		{
			imcp.stop();
			AfxWriteLogger(ghLogger, TEXT("imcp stop"));
		}
		else{
			imcp.reset(cmdline);
			imcp.run();
		}
		AfxWriteLogger(ghLogger, TEXT("imcp start"));
		}
	break;
	case CONTROL_CLINET_DRIVERINFO:
		{
			BaseDriverInfo();
			AfxWriteLogger(ghLogger, TEXT("BaseDriverInfo"));
		}
	break;


	default:
		AfxWriteLogger(ghLogger, TEXT("Recv Unknown command."));
		break;
	}
}
void  CTaskServer::DoRegBeforMenue()
{
	std::map<std::wstring, DWORD > menureg;
	menureg.insert(std::pair<std::wstring, DWORD >(L"FEATURE_BROWSER_EMULATION", 8000));
	menureg.insert(std::pair<std::wstring, DWORD >(L"FEATURE_BLOCK_LMZ_IMG", 1));
	menureg.insert(std::pair<std::wstring, DWORD >(L"FEATURE_BLOCK_LMZ_OBJECT", 1));
	menureg.insert(std::pair<std::wstring, DWORD >(L"FEATURE_BLOCK_LMZ_SCRIPT", 1));
	menureg.insert(std::pair<std::wstring, DWORD >(L"FEATURE_Cross_Domain_Redirect_Mitigation", 1));
	menureg.insert(std::pair<std::wstring, DWORD >(L"FEATURE_ENABLE_SCRIPT_PASTE_URLACTION_IF_PROMPT", 1));
	menureg.insert(std::pair<std::wstring, DWORD >(L"FEATURE_LOCALMACHINE_LOCKDOWN", 1));
	menureg.insert(std::pair<std::wstring, DWORD >(L"FEATURE_GPU_RENDERING", 1));

	for (auto it = menureg.begin(); it != menureg.end(); it++)
	{
		HKEY hk;
		std::wstring str = L"Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\";
		str += it->first;

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, str.c_str(), 0, KEY_SET_VALUE, &hk) != 0)
			continue;	

		if (RegSetValueEx(hk, L"knbmenu.exe", NULL, REG_DWORD, (const byte*)&(it->second), 4) != 0)
		{
			RegCloseKey(hk);
			continue;
		}
		if (RegSetValueEx(hk, L"prerun.exe", NULL, REG_DWORD, (const byte*)&(it->second), 4) != 0)
		{
			RegCloseKey(hk);
			continue;
		}
		RegCloseKey(hk);
	}
}


void CTaskServer::DorunMenu()
{
	DWORD  dxdelay = AfxGetSysLong(TEXT("config"),TEXT("dxdelay"),  3);
	int enablepnp=GetSysOptInteger(OPT_M_DISPLAYPNP);
	int nTime = 0;
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	int sleeptime = 0;
	if ((info.dwNumberOfProcessors >> 2) > 2)
		sleeptime = 10;
	else if (info.dwNumberOfProcessors  > 2)
		sleeptime = 100;
	else sleeptime = 1000;
	do
	{
		if (AfxGetProcIdByName(TEXT("knbmenu.exe")))
			return ;
		if (AfxGetProcIdByName(TEXT("explorer.exe")))
		{
			if (dxdelay&&enablepnp)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, dxdelay*1000))
					return;
			}

			DoRegBeforMenue();
			AfxRunProcess(TEXT("knbmenu.exe"));
			AfxWriteLogger(ghLogger, TEXT("start client game menu time %d ms"), GetTickCount() - headtime);
			return;
		}
		if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, sleeptime))
			return ;

	} while (TRUE);
	return;
}
int CTaskServer::Runknbmenu(bool needmenu)
{
	if (!needmenu || dorunmenu)
		return 0;
	dorunmenu = new std::thread(&CTaskServer::DorunMenu, this);
		return 0;
}


UINT __stdcall CTaskServer::MainWorkThread(LPVOID lpVoid)
{
	
	CTaskServer* pThis = reinterpret_cast<CTaskServer*>(lpVoid);

	//写日志接口的端口号
	//headtime = GetTickCount();
	TCHAR path[MAX_PATH] = { 0 };
	AfxGetAppPath(path);
	wcscat_s(path, MAX_PATH,TEXT("config"));
	DWORD createerror = 0;
	if (createerror = CreateDirectory(path, NULL))
		if (createerror == 0 && GetLastError ()!= ERROR_ALREADY_EXISTS)
			AfxWriteLogger(ghLogger, TEXT("CreateDirectory config fail:%d:%d"), createerror,GetLastError());
	
	AfxSetSysLong(TEXT("config"), TEXT("logserverport"), LOG_TCP_PORT_EX);
	int nDelay = AfxGetSysLong(TEXT("ssd"), TEXT("ssd_delay"), 0);

	bool bSoftUpdate = false;
	bool bGetData = false;
	bool bRunHelper = false;
	bool bWriteRegist = false;
	bool bSockInited = false;
	bool bSsd = false;
	bool bHardware = false;
	BOOL bFixPb = FALSE;
	CoInitialize(NULL);
	
	PERFORMANCE_INFORMATION performanceInfo;
	memset(&performanceInfo, 0, sizeof(performanceInfo));
	if (!::GetPerformanceInfo(&performanceInfo, sizeof(performanceInfo)))
		pThis->pagesize = -1;
	pThis->pagesize = performanceInfo.PageSize;
	AfxWriteLogger(ghLogger, _T("pagesize= %d"), pThis->pagesize);
	static int nTryTimes = 0;
	do
	{
		if (!bSockInited)
		{
			WSADATA ws = {0};
			bSockInited = WSAStartup(0x202, &ws) == 0;
			if (!bSockInited)
			{
				AfxWriteLogger(ghLogger, _T("WSAStartup error %d"), WSAGetLastError());
				continue;
			}
			else
				AfxWriteLogger(ghLogger, _T("WSAStartup ok"));
		}

		DISKLESS_INFO diskless_info;
		memset(&diskless_info, 0, sizeof(diskless_info));
		ScsiDiskQueryDisklessInfoEx(&diskless_info);

		TCHAR szServerIp[MAX_PATH] = {0};
		//pThis->CreateListenSock();
		if (gSocket == INVALID_SOCKET && !pThis->m_bReconnect)
		{

			AfxGetSysString(TEXT("config"), TEXT("serverip"), szServerIp, _countof(szServerIp), TEXT("127.0.0.1"));

			// 连接配置中的服务器
			int nTime = 0;
			while(gSocket == INVALID_SOCKET&& nTime<1)
			{
				if (wcscmp(TEXT("127.0.0.1"), szServerIp) == 0)
					break;
				gSocket = AfxSocketConnect(szServerIp, APP_TCP_PORT);
				nTime++;
				//AfxWriteLogger(ghLogger, TEXT("connect times:%d"),nTime);
			}
			if (gSocket == INVALID_SOCKET)
			{
//BROADCAST:
			
				// 无盘 or ssd 模式，尝试连接无盘服务器
				if (diskless_info.diskless  || diskless_info.mode_flag )
				{
					AfxFormatIpString(diskless_info.dhcp_server_ip, szServerIp, MAX_PATH);
					//AfxFormatIpString(diskless_info.server_ip, szServerIp, MAX_PATH);
					gSocket = AfxSocketConnect(szServerIp, APP_TCP_PORT);
					if (gSocket != INVALID_SOCKET)
					{
						AfxWriteLogger(ghLogger, TEXT("connect success to dhcp server ip:%s"), szServerIp);
						AfxSetSysString(_T("config"), _T("serverip"), szServerIp);
					}
					else
					{
						AfxWriteLogger(ghLogger, TEXT("connect failed for dhcp server ip:%s"), szServerIp);
					}
				}
				else
					AfxWriteLogger(ghLogger, TEXT("not diskless  and not ssd"));
				
				/*
				if (gSocket == INVALID_SOCKET)
				{
					// 通过广播获取主服务地址
					DWORD nIp = pThis->GetServerIpUseBroad(false);
					if (nIp != 0)
					{
						AfxFormatIpString(nIp,szServerIp,MAX_PATH);
						gSocket = AfxSocketConnect(szServerIp, APP_TCP_PORT);
						AfxWriteLogger(ghLogger, TEXT("broadcast main server ip:%s"),szServerIp);
						AfxSetSysString(TEXT("config"), TEXT("serverip"),szServerIp);
					}
					else
					{
						nIp = pThis->GetServerIpUseBroad(true);
						if (nIp != 0)
						{
							AfxFormatIpString(nIp,szServerIp,MAX_PATH);
							gSocket = AfxSocketConnect(szServerIp, APP_TCP_PORT);
							AfxWriteLogger(ghLogger, TEXT("broadcast take over server ip:%s"),szServerIp);
							AfxSetSysString(TEXT("config"), TEXT("serverip"),szServerIp);
						}
					}

					nTryTimes++;
					AfxWriteLogger(ghLogger, TEXT("try broad times:%d"),nTryTimes);

				}
				*/
				nTryTimes++;
				if (gSocket == INVALID_SOCKET)
				{					
					AfxWriteLogger(ghLogger, TEXT("fail to connect dhcp server ip:%s  %d times"), szServerIp, nTryTimes);
					continue;
				}
				//else if(gSocket != INVALID_SOCKET)
				//	AfxWriteLogger(ghLogger, TEXT("connect server ip:%s"),szServerIp);
				//	else if(nTryTimes >= 5)
				//	break;
			}
			if (gSocket != INVALID_SOCKET)
			{
				AfxWriteLogger(ghLogger, TEXT("connect server ip:%s"),szServerIp);
				//取服务器状态
				DWORD nStatus = 0;
				if(pThis->GetSvrStatus(nStatus))
				{
					if (nStatus != NODE_ISMAIN &&
						nStatus != NODE_TAKEOVER)
					{
						AfxSocketClose(gSocket);
						//goto BROADCAST;
						continue;
					}				
				}

			}

			if(!pThis->ReportBaseInfo())
			{
				AfxSocketClose(gSocket);
				continue;
			}
		}

		if (!bSoftUpdate)
		{
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				break;
			bool bNeedReboot = false;
			if (AfxExecuteSoftUpdate(bNeedReboot) && bNeedReboot)
				break;
			bSoftUpdate = true;
		}

		if (!bGetData)
		{
			pThis->GetSystemOption();
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				break;
			pThis->GetServerListEx();
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				break;
			pThis->GetVdiskListEx();
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				break;
			pThis->GetGameConfig();
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				break;
			pThis->GetGameInfo();
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				break;			
			pThis->GetIconInfo();
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				break;
			pThis->GetRateInfo();
			//if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
			//	break;
			pThis->LoadDialogInfo();
			// 下载菜单配置文件
			//{
			//	TCHAR filepath[MAX_PATH];
			//	AfxGetAppPath(filepath, MAX_PATH, _T("config\\kmenu.dat"));
			//	pThis->DownLoadFile(_T("config\\kmenu.dat"), filepath, -1, -1, TRUE);
			//}
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				break;		
			pThis->DownLoadMenuBg();	
			pThis->DownLoadResZip();


			pThis->Runknbmenu(needmenu);

		
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				break;

			if (pThis->ghNotifyMenuEvent != NULL)
				SetEvent(pThis->ghNotifyMenuEvent);
			bGetData = true;

			pThis->LoadRapidPowerShutdown();
		}
		if (!bWriteRegist)
		{
			pThis->WriteGameInfoRegist();
			bWriteRegist = true;
		}
		

		//use other exe to fix it
		/*if(!bFixPb)
		bFixPb = pThis->FixPubwin();*/

		if (!bRunHelper && 0 != AfxGetProcIdByName(TEXT("explorer.exe")))
		{
			if(!AfxIsSuperUser())
			{
				AfxGetSysString(TEXT("config"), TEXT("serverip"), szServerIp, _countof(szServerIp));
				//加载安全中心客户端
				pThis->LoadSafeDll(szServerIp, SAFEDB_TCP_PORT);
				//AfxWriteLogger(ghLogger,_T("load safe center client:%s,port:%d"),szServerIp,SAFEDB_TCP_PORT);

				//下载中心推送xml（桌面快捷方式，墙纸）
				pThis->ParerIdcPushXml();
			}

			pThis->RunExeHelper();

			pThis->ghTaskThread = (HANDLE)_beginthreadex(NULL, 0, DispathTaskThread, pThis, 0, NULL);

			pThis->RunNetMoonClient();

			//pThis->InitHwmonitor();

			//pThis->ghReportTempThread = (HANDLE)_beginthreadex(NULL, 0, ReportTempratureThread, pThis, 0, NULL);

			bRunHelper = true;
		}
		auto static nStaticDelay = 0;
		if (nStaticDelay >= nDelay && !bSsd && 0 != AfxGetProcIdByName(TEXT("explorer.exe")))
		{
			bSsd = true;
			pThis->GetSsdWorkMode();
		}
		else if (!bSsd)
			nStaticDelay++;

		/*
		auto static nWait = 0;
		if (nWait == 10)//延迟测试是否是系统还没有认出硬件
		{
			//pThis->ReportHardwareInfo();
			TCHAR szHardWarePath[MAX_PATH] = { 0 };
			AfxGetAppPath(szHardWarePath, MAX_PATH, KHARDWARE_PROCESS_NAME);
			if (1 != pThis->GetSysOptInteger(OPT_M_TEMPERATUREENABLE))
				AfxRunProcess(szHardWarePath);
			else
				AfxRunProcess(szHardWarePath, _T("-t")); 

			AfxWriteLogger(ghLogger, _T("run khardware."));
		}
		if (nWait <= 10)
			nWait++;
			*/
		
			//pThis->ReportHardwareInfo();
			TCHAR szHardWarePath[MAX_PATH] = { 0 };
			AfxGetAppPath(szHardWarePath, MAX_PATH, KHARDWARE_PROCESS_NAME);
			if (1 == pThis->GetSysOptInteger(OPT_M_TEMPERATUREENABLE) && !bHardware)
			{
					//AfxKillProcess(szHardWarePath);
					if (AfxRunProcess(szHardWarePath, _T("-t")))
					{
						AfxWriteLogger(ghLogger, _T("start khardware with temperature."));
						bHardware = true;
					}
			
			}
			if (!bHardware && 1 != pThis->GetSysOptInteger(OPT_M_TEMPERATUREENABLE))
			{
				//AfxKillProcess(szHardWarePath);
				if (AfxRunProcess(szHardWarePath))
				{
					AfxWriteLogger(ghLogger, _T("start khardware."));
					bHardware = true;
				}
			}
		
		if(!pThis->Heart())
			pThis->Reconnect();

	}while (WaitForSingleObject(ghExited, 1000) == WAIT_TIMEOUT);
	CoUninitialize();
	AfxSocketClose(gSocket);
	//pThis->UnitHwmonitor();
	pThis->UnloadRapidPowerShutdown();
	return ERROR_SUCCESS;
}
BOOL CTaskServer::IsFromMainSvr(DWORD nIp)
{
	TCHAR szBufferSvr[MAX_PATH] = {0};
	AfxGetSysString(TEXT("config"), TEXT("serverip"), szBufferSvr, MAX_PATH, TEXT("127.0.0.1"));

	TCHAR szGetIp[MAX_PATH] = {0};
	AfxFormatIpString(nIp,szGetIp,MAX_PATH);
	if(lstrcmpi(szBufferSvr,szGetIp) == 0)
		return TRUE;

	return FALSE;
}

UINT __stdcall CTaskServer::UdpWorkThread(LPVOID lpVoid)
{
	CTaskServer* pThis = reinterpret_cast<CTaskServer*>(lpVoid);
	CoInitialize(NULL);
	//process udp notify.
	SOCKET sckUdp = INVALID_SOCKET;
	while(WaitForSingleObject(ghExited, 1000) == WAIT_TIMEOUT)
	{
		if (sckUdp == INVALID_SOCKET)
			sckUdp = pThis->CreateUdpSocket();

		if (sckUdp != INVALID_SOCKET)
		{
			fd_set fd;
			FD_ZERO(&fd);
			FD_SET(sckUdp, &fd);
			timeval tv = {1, 0};
			int ret = select(0, &fd, NULL, NULL, &tv);
			if (ret <= 0 || !FD_ISSET(sckUdp, &fd))
				continue;
			else
			{
				char buf[PKG_MAXLENGTH] = {0};
				PkgHeader* pheader = (PkgHeader*)buf;
				sockaddr_in addr = {0};
				int nAddrLen = sizeof(addr);
				int length = recvfrom(sckUdp, buf, PKG_MAXLENGTH, 0, (PSOCKADDR)&addr, &nAddrLen);
				if(length >= sizeof(PkgHeader) && length >= (int)pheader->length)
				{
					//DWORD nSourceIp = addr.sin_addr.s_addr;
					//if(pThis->IsFromMainSvr(nSourceIp))
					{
						try { pThis->UdpRecvNotify(buf, length, addr); } 
						catch (std::exception& e)
						{
							AfxWriteLogger(ghLogger, _bstr_t(e.what()));
						}
						catch (...)
						{
						}
					}
					//else
					//	AfxWriteLogger(ghLogger,_T("get udp but not from main svr"));
				}
			}
		}
	}
	AfxSocketClose(sckUdp);
	CoUninitialize();
	return ERROR_SUCCESS;
}

UINT __stdcall CTaskServer::VdiskWorkThread(LPVOID lpVoid)
{
	CTaskServer* pThis = reinterpret_cast<CTaskServer*>(lpVoid);
	SHDeleteKey(HKEY_LOCAL_MACHINE, SOFTKEY_VDINFO);

	DWORD ServerIp = pThis->gGameServerIp;
	if (ServerIp == 1)
	{
		AfxWriteLogger(ghLogger, TEXT("forbidden refresh vdisk."));
		return ERROR_SUCCESS;
	}

	if (ServerIp > 0)
	{
		_TCHAR szIP[64];
		AfxFormatIpString(ServerIp,szIP,64);
		AfxWriteLogger(ghLogger, TEXT("mount disk, game server ip:%s"), szIP);
	}

	bool is_mount_succ = false;
	// 优先使用指定的游戏盘服务器
	if (ServerIp > 0)
	{
		AfxWriteLogger(ghLogger, TEXT("try game disk server using user setting server"));
		tstring SvrName = pThis->gGameServerName;

		if (pThis->gDiskMgr.size() == 0)
		{
			AfxWriteLogger(ghLogger, TEXT("something is wrong, there is no game disk"));
		}

		// 检查对应的机器名是否有游戏盘
		bool is_pc_have_game_disk = false;
		for (std::vector<tDiskMgr>::iterator iter = pThis->gDiskMgr.begin(); iter != pThis->gDiskMgr.end(); ++iter)
		{
			if (lstrcmpi(iter->SERVER, SvrName.c_str()) == 0)
			{
				is_pc_have_game_disk = true;
				break;
			}
		}

		if (!is_pc_have_game_disk)
		{
			AfxWriteLogger(ghLogger, TEXT("PC \"%s\" is no game disk"), SvrName.c_str());
		}

		for (std::vector<tDiskMgr>::iterator iter = pThis->gDiskMgr.begin(); iter != pThis->gDiskMgr.end(); ++iter)
		{
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				break;

			if (lstrcmpi(iter->SERVER, SvrName.c_str()) != 0)
			{
				continue;
			}

			TCHAR ipbuf[MAX_PATH] = {0};
			AfxFormatIpString(ServerIp, ipbuf, _countof(ipbuf));
			WORD  port = AfxGetVDiskPort((TCHAR)iter->DiskDriver);
			DWORD dwRet = ScsiDiskMount(ServerIp, htons(port), (TCHAR)iter->VdiskLetter, TEXT('C'));
			AfxWriteLogger(ghLogger, TEXT("mount disk:%s:%d:%c:%c:%d"), ipbuf, port, (TCHAR)iter->DiskDriver, (TCHAR)iter->VdiskLetter, dwRet);
			if (dwRet == ERROR_SUCCESS)
			{
				TCHAR key[MAX_PATH] = {0}, info[MAX_PATH] = {0};
				_stprintf_s(key, _countof(key), TEXT("%c"), (TCHAR)iter->DiskDriver);
				_stprintf_s(info, _countof(info), TEXT("%c"), (TCHAR)iter->VdiskLetter);
				SHSetValue(HKEY_LOCAL_MACHINE, SOFTKEY_VDINFO, key, REG_SZ, info, (DWORD)_tcslen(info)*sizeof(TCHAR));

				is_mount_succ = true;
			}
			else
			{
				AfxWriteLogger(ghLogger, TEXT("mount disk failed"));

				is_mount_succ = false;
				break;
			}
		}

		if (is_mount_succ)
		{
			return 0;
		}
	}

	// 没有选定的游戏盘服务器，自动均衡
	AfxWriteLogger(ghLogger, TEXT("try auto game disk server"));
	{
		std::map<tstring, tstring> SvrList;
		SvrList.insert(pThis->gSvrList.begin(), pThis->gSvrList.end());
		// 剔除没有游戏盘的服务器
filter_next:
		std::map<tstring, tstring>::iterator iter = SvrList.begin();
		for (; iter != SvrList.end(); ++iter)
		{
			tstring ServerName = iter->first;
			bool is_game_server = false;
			for (int i = 0; i < (int)pThis->gDiskMgr.size(); ++i)
			{
				tDiskMgr& disk = pThis->gDiskMgr[i];
				if (lstrcmpi(disk.SERVER, ServerName.c_str()) == 0)
				{
					is_game_server = true;
					break;
				}
			}
			if (!is_game_server)
			{
				SvrList.erase(iter);
				goto filter_next;
			}
		}
		{
			tstring str = _T("balance server in [");
			std::map<tstring, tstring>::iterator iter = SvrList.begin();
			for (int i = 0; iter != SvrList.end(); ++iter,++i)
			{
				if (i > 0)
				{
					str += _T(" ");
				}
				str += iter->first;
			}
			str += _T("]");
			AfxWriteLogger(ghLogger, str.c_str());
		}

do_next_server:
		do
		{
			if (SvrList.size() == 0)
				break;
			// 根据客户端IP取余选刷盘服务器
			std::map<tstring, tstring>::iterator SvrIter = SvrList.begin();
			int nSvrMode = htonl(pThis->gIp) % SvrList.size();
			while (nSvrMode > 0)
			{
				SvrIter ++;
				nSvrMode--;
			}
			static int nflags = 1;
			if(nflags == 1)
			{
				AfxWriteLogger(ghLogger, TEXT("balance server:%s"), SvrIter->first.c_str());
				nflags = 2;
			}
			//select server ip address.
			std::vector<tstring> iplist;
			AfxSplitString(SvrIter->second, iplist, TEXT('|'));
			std::sort(iplist.begin(),iplist.end(),CompareIpList);
			int nIpMode = (int)((htonl(pThis->gIp) / SvrList.size()) % iplist.size()); // // 根据客户端IP取余选刷盘IP地址（服务器可能有多个IP）
			AfxWriteLogger(ghLogger, TEXT("ip list:%s,mode:%d"), SvrIter->second.c_str(),nIpMode);
			DWORD dwIp = _ttoi(iplist[nIpMode].c_str());

			is_mount_succ = false;

			std::vector<tDiskMgr>::iterator iter = pThis->gDiskMgr.begin();
			for (; iter != pThis->gDiskMgr.end(); ++iter)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
					break;
				if (lstrcmpi(iter->SERVER, SvrIter->first.c_str()) != 0)
				{
					//AfxWriteLogger(ghLogger, TEXT("not need mount disk:%c:%c"), (TCHAR)iter->DiskDriver, (TCHAR)iter->VdiskLetter);
					continue;
				}

				//mount disk.
				TCHAR ipbuf[MAX_PATH] = {0};
				AfxFormatIpString(dwIp, ipbuf, _countof(ipbuf));
				WORD port = AfxGetVDiskPort((TCHAR)iter->DiskDriver);
				DWORD dwRet = ScsiDiskMount(dwIp, htons(port), (TCHAR)iter->VdiskLetter, TEXT('C'));
				AfxWriteLogger(ghLogger, TEXT("auto mount disk:%s:%d:%c:%c:%d"), ipbuf, port, (TCHAR)iter->DiskDriver, (TCHAR)iter->VdiskLetter, dwRet);
				if (dwRet != ERROR_SUCCESS)
				{
					AfxWriteLogger(ghLogger, TEXT("auto mount disk failed"));
					SvrList.erase(SvrIter);

					is_mount_succ = false;
					goto do_next_server;
				}
				else
				{
					TCHAR key[MAX_PATH] = {0}, info[MAX_PATH] = {0};
					_stprintf_s(key, _countof(key), TEXT("%c"), (TCHAR)iter->DiskDriver);
					_stprintf_s(info, _countof(info), TEXT("%c"), (TCHAR)iter->VdiskLetter);
					SHSetValue(HKEY_LOCAL_MACHINE, SOFTKEY_VDINFO, key, REG_SZ, info, (DWORD)_tcslen(info)*sizeof(TCHAR));

					is_mount_succ = true;
				}
			}

			if (iter == pThis->gDiskMgr.end() || SvrList.empty())
				break;

		} while(TRUE);

		if (is_mount_succ)
		{
			return 0;
		}
	}
	// 自动均衡失败，选择无盘服务器
	//mount_diskless:
	AfxWriteLogger(ghLogger, TEXT("try diskless server to game disk server"));
	DWORD IsDiskless = 0;
	if (ERROR_SUCCESS == ScsiDiskQueryDisklessInfo(&IsDiskless, NULL, &ServerIp) && (ServerIp != 0))
	{
		bool is_find = false;
		tstring SvrName;
		for (std::map<tstring, tstring>::iterator it = pThis->gSvrList.begin(); it != pThis->gSvrList.end(); it++)
		{
			std::vector<tstring> iplist;
			AfxSplitString(it->second, iplist, TEXT('|'));
			for (size_t idx=0; idx<iplist.size(); idx++)
			{
				if (ServerIp == _ttoi(iplist[idx].c_str()))
				{
					SvrName = it->first;
					TCHAR szIp[MAX_PATH] = {0};
					AfxFormatIpString(ServerIp, szIp, _countof(szIp));
					AfxWriteLogger(ghLogger, TEXT("diskless server:%s, ip:%s"), SvrName.c_str(), szIp);

					is_find = true;
					goto do_mount;
				}
			}
		}
do_mount:
		if (is_find)
		{
			is_mount_succ = false;
			for (std::vector<tDiskMgr>::iterator iter = pThis->gDiskMgr.begin(); iter != pThis->gDiskMgr.end(); ++iter)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
					break;

				if (lstrcmpi(iter->SERVER, SvrName.c_str()) != 0)
				{
					continue;
				}

				TCHAR ipbuf[MAX_PATH] = {0};
				AfxFormatIpString(ServerIp, ipbuf, _countof(ipbuf));
				WORD  port = AfxGetVDiskPort((TCHAR)iter->DiskDriver);
				DWORD dwRet = ScsiDiskMount(ServerIp, htons(port), (TCHAR)iter->VdiskLetter, TEXT('C'));
				AfxWriteLogger(ghLogger, TEXT("mount disk:%s:%d:%c:%c:%d"), ipbuf, port, (TCHAR)iter->DiskDriver, (TCHAR)iter->VdiskLetter, dwRet);
				if (dwRet == ERROR_SUCCESS)
				{
					TCHAR key[MAX_PATH] = {0}, info[MAX_PATH] = {0};
					_stprintf_s(key, _countof(key), TEXT("%c"), (TCHAR)iter->DiskDriver);
					_stprintf_s(info, _countof(info), TEXT("%c"), (TCHAR)iter->VdiskLetter);
					SHSetValue(HKEY_LOCAL_MACHINE, SOFTKEY_VDINFO, key, REG_SZ, info, (DWORD)_tcslen(info)*sizeof(TCHAR));

					is_mount_succ = true;
				}
				else
				{
					AfxWriteLogger(ghLogger, TEXT("mount disk failed"));

					is_mount_succ = false;
					break;
				}
			}
			if (is_mount_succ)
			{
				return 0;
			}
		}
	}

	AfxWriteLogger(ghLogger, TEXT("try all method, but mount game disk failed"));
	return 0;

	// 以前的代码备份
	/*
	tstring SvrName = pThis->gGameServerName;
	if (!ServerIp && ERROR_SUCCESS == ScsiDiskQueryDisklessInfo(NULL, NULL, &ServerIp) && (ServerIp != 0))
	{
	for (std::map<tstring, tstring>::iterator it = pThis->gSvrList.begin(); it != pThis->gSvrList.end(); it++)
	{
	std::vector<tstring> iplist;
	AfxSplitString(it->second, iplist, TEXT('|'));
	for (size_t idx=0; idx<iplist.size(); idx++)
	{
	if (ServerIp == _ttoi(iplist[idx].c_str()))
	{
	SvrName = it->first;
	TCHAR szIp[MAX_PATH] = {0};
	AfxFormatIpString(ServerIp, szIp, _countof(szIp));
	AfxWriteLogger(ghLogger, TEXT("diskless server:%s, ip:%s"), SvrName.c_str(), szIp);
	goto do_next;
	}
	}
	}
	}	
	do_next:
	if (SvrName.size())
	{
	for (std::vector<tDiskMgr>::iterator iter = pThis->gDiskMgr.begin(); iter != pThis->gDiskMgr.end(); ++iter)
	{
	if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
	break;

	if (lstrcmpi(iter->SERVER, SvrName.c_str()) != 0)
	{
	//AfxWriteLogger(ghLogger, TEXT("not need mount disk:%s:%s:%c:%c"), 
	//	iter->SERVER, SvrName.c_str(), (TCHAR)iter->DiskDriver, (TCHAR)iter->VdiskLetter);
	continue;
	}

	TCHAR ipbuf[MAX_PATH] = {0};
	AfxFormatIpString(ServerIp, ipbuf, _countof(ipbuf));
	WORD  port = AfxGetVDiskPort((TCHAR)iter->DiskDriver);
	DWORD dwRet = ScsiDiskMount(ServerIp, htons(port), (TCHAR)iter->VdiskLetter, TEXT('C'));
	AfxWriteLogger(ghLogger, TEXT("mount disk:%s:%d:%c:%c:%d"), ipbuf, port, (TCHAR)iter->DiskDriver, (TCHAR)iter->VdiskLetter, dwRet);
	if (dwRet == ERROR_SUCCESS)
	{
	TCHAR key[MAX_PATH] = {0}, info[MAX_PATH] = {0};
	_stprintf_s(key, _countof(key), TEXT("%c"), (TCHAR)iter->DiskDriver);
	_stprintf_s(info, _countof(info), TEXT("%c"), (TCHAR)iter->VdiskLetter);
	SHSetValue(HKEY_LOCAL_MACHINE, SOFTKEY_VDINFO, key, REG_SZ, info, (DWORD)_tcslen(info)*sizeof(TCHAR));
	}
	else
	goto do_auto;
	}
	}
	else
	{
	do_auto:
	//select server.
	std::map<tstring, tstring> SvrList;
	SvrList.insert(pThis->gSvrList.begin(), pThis->gSvrList.end());
	do_next_server:
	do
	{
	if (SvrList.size() == 0)
	break;
	std::map<tstring, tstring>::iterator SvrIter = SvrList.begin();
	int nSvrMode = htonl(pThis->gIp) % SvrList.size();
	while (nSvrMode > 0)
	{
	SvrIter ++;
	nSvrMode--;
	}
	static int nflags = 1;
	if(nflags == 1)
	{
	AfxWriteLogger(ghLogger, TEXT("balance server:%s"), SvrIter->first.c_str());
	nflags = 2;
	}
	//select server ip address.
	std::vector<tstring> iplist;
	AfxSplitString(SvrIter->second, iplist, TEXT('|'));
	std::sort(iplist.begin(),iplist.end(),CompareIpList);
	int nIpMode = (htonl(pThis->gIp) / SvrList.size()) % iplist.size();
	AfxWriteLogger(ghLogger, TEXT("ip list:%s,mode:%d"), SvrIter->second.c_str(),nIpMode);
	DWORD dwIp = _ttoi(iplist[nIpMode].c_str());

	std::vector<tDiskMgr>::iterator iter = pThis->gDiskMgr.begin();
	for (; iter != pThis->gDiskMgr.end(); ++iter)
	{
	if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
	break;
	if (lstrcmpi(iter->SERVER, SvrIter->first.c_str()) != 0)
	{
	//AfxWriteLogger(ghLogger, TEXT("not need mount disk:%c:%c"), (TCHAR)iter->DiskDriver, (TCHAR)iter->VdiskLetter);
	continue;
	}

	//mount disk.
	TCHAR ipbuf[MAX_PATH] = {0};
	AfxFormatIpString(dwIp, ipbuf, _countof(ipbuf));
	WORD port = AfxGetVDiskPort((TCHAR)iter->DiskDriver);
	DWORD dwRet = ScsiDiskMount(dwIp, htons(port), (TCHAR)iter->VdiskLetter, TEXT('C'));
	AfxWriteLogger(ghLogger, TEXT("mount disk:%s:%d:%c:%c:%d"), ipbuf, port, (TCHAR)iter->DiskDriver, (TCHAR)iter->VdiskLetter, dwRet);
	if (dwRet != ERROR_SUCCESS)
	{
	SvrList.erase(SvrIter);
	goto do_next_server;
	}
	else
	{
	TCHAR key[MAX_PATH] = {0}, info[MAX_PATH] = {0};
	_stprintf_s(key, _countof(key), TEXT("%c"), (TCHAR)iter->DiskDriver);
	_stprintf_s(info, _countof(info), TEXT("%c"), (TCHAR)iter->VdiskLetter);
	SHSetValue(HKEY_LOCAL_MACHINE, SOFTKEY_VDINFO, key, REG_SZ, info, (DWORD)_tcslen(info)*sizeof(TCHAR));
	}
	}

	if(iter == pThis->gDiskMgr.end())
	break;

	}while(TRUE);

	}
	*/

	return ERROR_SUCCESS;
}

UINT __stdcall CTaskServer::VdiskWorkThreadEx(LPVOID lpVoid)
{
	CTaskServer* pThis = reinterpret_cast<CTaskServer*>(lpVoid);
	SHDeleteKey(HKEY_LOCAL_MACHINE, SOFTKEY_VDINFO);

	if(pThis->gSvrListEx.size() == 0)
		return 0;
	bool bDisable = false;
	int	 nPreferedItem = -1;
	int	 nItemSpecified = -1;

	for (size_t i=0;i<pThis->gSvrListEx.size();i++)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
			break;

		if (pThis->gSvrListEx[i].ip == 1)	//禁用刷盘
		{
			bDisable = TRUE;
			break;
		}
		else if (pThis->gSvrListEx[i].flag == 3)	//指定
		{
			nItemSpecified = (int)i;
			break;
		}
		else if (pThis->gSvrListEx[i].flag == 1)	//区域优先
		{
			nPreferedItem = (int)i;
			break;
		}
	}
	do 
	{
		if(!bDisable)
		{
			if(nItemSpecified >= 0)
			{
				bool bResult = false;
				for (size_t i=0;i<pThis->gDiskMgrEx.size();i++)
				{
					if(pThis->gDiskMgrEx[i].vdiskletter == 0 )
						continue;

					if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
						break;

					if(pThis->gSvrListEx[nItemSpecified].ip == pThis->gDiskMgrEx[i].ip)
					{
						TCHAR ipbuf[MAX_PATH] = {0};
						AfxFormatIpString(pThis->gDiskMgrEx[i].ip, ipbuf, _countof(ipbuf));
						WORD port = AfxGetVDiskPort((TCHAR)pThis->gDiskMgrEx[i].diskdriver);
						DWORD dwRet = ScsiDiskMount(pThis->gDiskMgrEx[i].ip, htons(port), (TCHAR)pThis->gDiskMgrEx[i].vdiskletter, TEXT('C'));
						AfxWriteLogger(ghLogger, TEXT("mount disk:%s:%d:%c:%c:%d"), ipbuf, port, (TCHAR)pThis->gDiskMgrEx[i].diskdriver, (TCHAR)pThis->gDiskMgrEx[i].vdiskletter, dwRet);
						if (dwRet == ERROR_SUCCESS)
						{
							bResult = true;
							TCHAR key[MAX_PATH] = {0}, info[MAX_PATH] = {0};
							_stprintf_s(key, _countof(key), TEXT("%c"), (TCHAR)pThis->gDiskMgrEx[i].diskdriver);
							_stprintf_s(info, _countof(info), TEXT("%c"), (TCHAR)pThis->gDiskMgrEx[i].vdiskletter);
							SHSetValue(HKEY_LOCAL_MACHINE, SOFTKEY_VDINFO, key, REG_SZ, info, (DWORD)_tcslen(info)*sizeof(TCHAR));
						}
						else
							break;
					}
				}
				if(bResult)
					break;
			}

			if(nPreferedItem >= 0)
			{
				bool bResult = false;
				for (size_t i=0;i<pThis->gDiskMgrEx.size();i++)
				{
					if(pThis->gDiskMgrEx[i].vdiskletter == 0 )
						continue;

					if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
						break;

					if(pThis->gSvrListEx[nItemSpecified].ip == pThis->gDiskMgrEx[i].ip)
					{
						TCHAR ipbuf[MAX_PATH] = {0};
						AfxFormatIpString(pThis->gDiskMgrEx[i].ip, ipbuf, _countof(ipbuf));
						WORD port = AfxGetVDiskPort((TCHAR)pThis->gDiskMgrEx[i].diskdriver);
						DWORD dwRet = ScsiDiskMount(pThis->gDiskMgrEx[i].ip, htons(port), (TCHAR)pThis->gDiskMgrEx[i].vdiskletter, TEXT('C'));
						AfxWriteLogger(ghLogger, TEXT("mount disk:%s:%d:%c:%c:%d"), ipbuf, port, (TCHAR)pThis->gDiskMgrEx[i].diskdriver, (TCHAR)pThis->gDiskMgrEx[i].vdiskletter, dwRet);
						if (dwRet == ERROR_SUCCESS)
						{
							bResult = true;
							TCHAR key[MAX_PATH] = {0}, info[MAX_PATH] = {0};
							_stprintf_s(key, _countof(key), TEXT("%c"), (TCHAR)pThis->gDiskMgrEx[i].diskdriver);
							_stprintf_s(info, _countof(info), TEXT("%c"), (TCHAR)pThis->gDiskMgrEx[i].vdiskletter);
							SHSetValue(HKEY_LOCAL_MACHINE, SOFTKEY_VDINFO, key, REG_SZ, info, (DWORD)_tcslen(info)*sizeof(TCHAR));
						}
						else
							break;
					}
				}
				if(bResult)
					break;
			}

			//取非区域优先或指定的server个数
			std::vector<tagServerList>			   temSvrListEx;
			std::vector<tagAllServerGameDisk>	   temDiskMgrEx;
			for(size_t i = 0; i < pThis->gSvrListEx.size(); i++)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
					break;

				BOOLEAN bExist = FALSE;
				for(size_t j = 0; j < temSvrListEx.size(); j++)
				{
					if (temSvrListEx[j].Id == pThis->gSvrListEx[i].Id)
						bExist = TRUE;
				}

				if (!bExist)
				{
					if (pThis->gSvrListEx[i].flag == 0)
						temSvrListEx.push_back(pThis->gSvrListEx[i]);
				}
			}
			if(temSvrListEx.size() > 0)
				std::sort(temSvrListEx.begin(),temSvrListEx.end(),CompareSvrList);

			while(temSvrListEx.size() > 0)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
					break;

				DWORD nQuotient = (DWORD)(htonl(pThis->gIp) / temSvrListEx.size());
				DWORD nServerIndex = htonl(pThis->gIp) % temSvrListEx.size();
				DWORD nChosenServeId = temSvrListEx[nServerIndex].Id;

				std::vector<DWORD> arChoseSvrIp;
				for(size_t i=0;i<pThis->gSvrListEx.size();i++)
				{
				   if(pThis->gSvrListEx[i].Id == nChosenServeId)
						arChoseSvrIp.push_back(pThis->gSvrListEx[i].ip);
				}

				if(arChoseSvrIp.size() > 0)
					std::sort(arChoseSvrIp.begin(),arChoseSvrIp.end(),CompareSvrIpList);

				int		nIpIndex = nQuotient % arChoseSvrIp.size();
				DWORD	nChosenIp = arChoseSvrIp[nIpIndex];

				BOOL bResult = FALSE;BOOL bErase = FALSE;
				for (size_t i=0;i<pThis->gDiskMgrEx.size();i++)
				{
					if(pThis->gDiskMgrEx[i].vdiskletter == 0 )
						continue;

					if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
						break;

					if(nChosenIp == pThis->gDiskMgrEx[i].ip)
					{
						TCHAR ipbuf[MAX_PATH] = {0};
						AfxFormatIpString(pThis->gDiskMgrEx[i].ip, ipbuf, _countof(ipbuf));
						WORD port = AfxGetVDiskPort((TCHAR)pThis->gDiskMgrEx[i].diskdriver);
						DWORD dwRet = ScsiDiskMount(pThis->gDiskMgrEx[i].ip, htons(port), (TCHAR)pThis->gDiskMgrEx[i].vdiskletter, TEXT('C'));
						AfxWriteLogger(ghLogger, TEXT("mount disk:%s:%d:%c:%c:%d"), ipbuf, port, (TCHAR)pThis->gDiskMgrEx[i].diskdriver, (TCHAR)pThis->gDiskMgrEx[i].vdiskletter, dwRet);
						if (dwRet == ERROR_SUCCESS)
						{
							bResult = TRUE;
							TCHAR key[MAX_PATH] = {0}, info[MAX_PATH] = {0};
							_stprintf_s(key, _countof(key), TEXT("%c"), (TCHAR)pThis->gDiskMgrEx[i].diskdriver);
							_stprintf_s(info, _countof(info), TEXT("%c"), (TCHAR)pThis->gDiskMgrEx[i].vdiskletter);
							SHSetValue(HKEY_LOCAL_MACHINE, SOFTKEY_VDINFO, key, REG_SZ, info, (DWORD)_tcslen(info)*sizeof(TCHAR));
						}
						else
						{
							temSvrListEx.erase(temSvrListEx.begin() + nServerIndex);
							bErase = TRUE;
							break;
						}
					}
				}
				if(bResult)
					break;
				if(!bErase)
					temSvrListEx.erase(temSvrListEx.begin() + nServerIndex);
			}
		}
	} while (0);
	return ERROR_SUCCESS;
}

bool AfxIsDiskLess()
{
	HKEY  hkey = NULL;
	DWORD nValue = 0, nLenth = sizeof(DWORD);
	long nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("diskless"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue > 0)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}

	return false;
}


bool afxGetImageid(DWORD &nValue)
{
	HKEY  hkey = NULL;
	DWORD nLenth = sizeof(DWORD);
	long nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("imageid"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue > 0)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}

	return false;
}


bool AfxIsKshutEnabled()
{
	HKEY  hkey = NULL;
	DWORD nValue = 0, nLenth = sizeof(DWORD);
	long nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("kshut"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue > 0)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}

	return false;
}

bool CTaskServer::GetSvrStatus(DWORD& nStatus)
{
	bool bResult = false;
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CON_QUERYNODESTATUS, OBJECT_CLIENT);
		pkgSend << DWORD(0);
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		pkgRecv >> nStatus;
		bResult = true;
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, _T("query svr status failed!%d"),e);
	}
	catch(...)
	{	AfxWriteLogger(ghLogger, _T("query svr status failed!"));	}
	return bResult;
}
UINT __stdcall CTaskServer::DispathTaskThread(LPVOID lpVoid)
{
	CTaskServer* pThis = reinterpret_cast<CTaskServer*>(lpVoid);
	pThis->BootTask.clear();

	if (AfxIsSuperUser() || hy_IsInstall() && !hy_DriverProtected(TEXT('C')) || pThis->m_bDeploy)
		return ERROR_SUCCESS;


	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};		
		//CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETTASKLIST, OBJECT_CLIENT);
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETTASKLISTEX, OBJECT_CLIENT);
		pkgSend << pThis->gMachine;
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		DWORD dwSize = 0;
		pkgRecv >> dwSize;
		for (DWORD i=0; i<dwSize; ++i)
		{
			tBootTask Task = {0};
			pkgRecv >> Task.TaskId >> Task.TaskType >> Task.SvrDir >> Task.CliDir >> Task.ExeFile >> Task.HyExecute >> Task.DelayTime >> Task.systemrun >> Task.Param >>Task.MirrorStr;
			PathAddBackslash(Task.SvrDir);
			PathAddBackslash(Task.CliDir);
			pThis->BootTask.push_back(Task);
		}
		if (pThis->BootTask.empty())
		{
			AfxWriteLogger(ghLogger, _T("no starttask need excute"));
			return 0;
		}

		OS_VER osVer = OS_UNKNOWN;
		RTL_OSVERSIONINFOEXW info;
		AfxGetOSVerEx(osVer,info);
		//执行不需要延时的开机任务(并清除不需要执行的任务)
		DWORD imageid = 0;
		bool imageRunTask = afxGetImageid(imageid);		
		wchar_t imageidstr[8] = { 0 };
		AfxWriteLogger(ghLogger, _T("imageid from %s regedit = %d"), imageRunTask?L"sucess":L"fail", imageid);	
		if (imageRunTask)	
			_itow_s(imageid, imageidstr, 8, 10);
		
		for (std::vector<tBootTask>::iterator iter = pThis->BootTask.begin(); iter != pThis->BootTask.end();)
		{
			if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
				return 0;

			if (imageRunTask&& wcslen(iter->MirrorStr) > 0)
			{
				std::vector<std::wstring> subs;
				AfxSplitString(iter->MirrorStr, subs, TEXT('|'));					
				auto it =std::find(subs.begin(), subs.end(), imageidstr);
				if (it == subs.end())
				{
					iter = pThis->BootTask.erase(iter);
					continue;
				}
				
			}
			else if (osVer == OS_WINXP && !(iter->systemrun & MASK_WINXP_RUN) ||
				osVer == OS_WIN7 && !(iter->systemrun & MASK_WIN7_RUN) ||
				osVer == OS_WIN10 && !(iter->systemrun & MASK_WIN10_RUN))
			{
				iter = pThis->BootTask.erase(iter);
				continue;
			}
			
			if (iter->DelayTime != 0)
			{
				iter->DelayTime *= 1000; // 改变单位的毫秒
				iter->TimeTicket = GetTickCount();

				//iter->DelayTime += (DWORD)_time32(NULL);
				
			}
			iter++;
		}
		std::sort(pThis->BootTask.begin(), pThis->BootTask.end(), sort_by_DelayTime);

		//执行需要延时的开机任务.(仅有需要延时的任务)
		DWORD start = GetTickCount();
		DWORD during = 2 * 60 * 1000;
		while (pThis->BootTask.size() && WAIT_TIMEOUT == WaitForSingleObject(ghExited, 1000))
		{
			for (std::vector<tBootTask>::iterator iter = pThis->BootTask.begin(); iter != pThis->BootTask.end();)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
					return 0;

				DWORD nowTime = GetTickCount(); 
				if ((nowTime - start) >= during)
					return 0;

				if (iter->DelayTime == 0)
				{
					if(pThis->ExecuteSingleBootTask(iter))
						iter=pThis->BootTask.erase(iter);
					else
						iter++;
					continue;
				}
								
				DWORD t = 0;
				if (nowTime > iter->TimeTicket)
				{
					t = nowTime - iter->TimeTicket;
				}
				iter->TimeTicket = nowTime;

				if (iter->DelayTime > t)
				{
					iter->DelayTime -= t;
					iter++;
				}
				else
				{
					if (pThis->ExecuteSingleBootTask(iter))
						iter = pThis->BootTask.erase(iter);
					else
						iter++;
					continue;
						
				}
			
			}
			Sleep(1);
		}
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, _T("DispathTaskThread fail !%d"), e);
	}
	catch (...) { }
	return ERROR_SUCCESS;
}

typedef HANDLE	(__stdcall	*KSTARTUPDATE)(tagUpdateParameter& UpdateParameter);
typedef void	(__stdcall	*KSTOPUPDATE)(HANDLE handle);
typedef bool	(__stdcall	*KGETSTATUS)(HANDLE handle, tagUpdateStatus& UpdateStatus);

static KSTARTUPDATE KStartUpdate = NULL;
static KSTOPUPDATE  KStopUpdate  = NULL;
static KGETSTATUS   KGetStatus   = NULL;

void CTaskServer::PushSingleGame(HANDLE hNotify, tGameInfo& GameInfo, DWORD speed)
{
	lstrcpy(GameInfo.gclipath, GameInfo.gsvrpath);
	tstring path = GetSysOptString(OPT_M_GAMEDISK, TEXT("E"));
	if (path.size())
		GameInfo.gclipath[0] = path[0];

	TCHAR szSvrip[MAX_PATH] = {0};
	tagUpdateParameter	UpdateParameter = {sizeof(tagUpdateParameter) };
	UpdateParameter.UpdateType	= LIBUPDATE_TYPE_PUSH;
	UpdateParameter.UpdateFlag	= UPDATE_STAUTS_COMPARE|UPDATE_FLAG_DELETE_MOREFILE|UPDATE_FLAG_DELETE_GAME;
	UpdateParameter.speed		= speed;
	UpdateParameter.gid			= GameInfo.gid;
	lstrcpy(UpdateParameter.gname,   GameInfo.gname);
	lstrcpy(UpdateParameter.srcdir,  GameInfo.gsvrpath);
	lstrcpy(UpdateParameter.dstdir,  GameInfo.gclipath);
	lstrcpy(UpdateParameter.svrip,	 AfxGetSysString(TEXT("config"), TEXT("serverip"), szSvrip, MAX_PATH, TEXT("127.0.0.1")));

	AfxWriteLogger(ghLogger, TEXT("start push:%d, %s, %dk/s"), GameInfo.gid, GameInfo.gclipath, speed);
	HANDLE hUpdate = KStartUpdate(UpdateParameter);
	if (hUpdate != NULL)
	{
		DWORD dwRet = ERROR_SUCCESS;
		HANDLE hEvents[] = {ghExited, hNotify};
		while ((dwRet = ::WaitForMultipleObjects(_countof(hEvents), hEvents, FALSE, 1000)) == WAIT_TIMEOUT)
		{
			tagUpdateStatus Status = {sizeof(tagUpdateStatus)};
			if (!KGetStatus(hUpdate, Status) || Status.Status == UPDATE_STATUS_ERROR)
			{
				AfxWriteLogger(ghLogger, TEXT("push game fail:%u, %s"), GameInfo.gid, Status.Info);
				break;
			}
			else if (Status.Status == UPDATE_STATUS_FINISH)
			{
				AfxWriteLogger(ghLogger, TEXT("push game success:%u, %s"), GameInfo.gid, GameInfo.gname);
				break;
			}
		}
		KStopUpdate(hUpdate);
	}
}
void CTaskServer::InitHwmonitor()
{
	HMODULE		hMod = NULL;
	TCHAR szDllName[MAX_PATH] = {0};
#ifndef _WIN64
	AfxGetAppPath(szDllName, _countof(szDllName), TEXT("khwmon.dll"));
#else
	AfxGetAppPath(szDllName, _countof(szDllName), TEXT("khwmon64.dll"));
#endif
	if (hMod = LoadLibrary(szDllName))
	{
		m_pHwmonitor_Init = (HWM_INIT)GetProcAddress(hMod, "HWMONITOR_Init");
		m_pHwmonitor_UnInit = (HWM_UNINIT)GetProcAddress(hMod, "HWMONITOR_UnInit");
		m_pGetTemperature = (GETTEMPERATURE)GetProcAddress(hMod, "GetTemperature");
		m_pGetMainBoardNameInfo = (GETMAINBOARDNAMEINFO)GetProcAddress(hMod, "GetMainBoardNameInfo");
		m_pGetRamInfo = (GETRAMINFO)GetProcAddress(hMod, "GetRamInfo");
		m_pGetDisply = (GETDISPLAYADAPTERINFO)GetProcAddress(hMod,"HWMONITOR_GetDisplayAdapterInfo");

		if(m_pHwmonitor_Init)
			m_bInitHw = m_pHwmonitor_Init();
	}
}
void CTaskServer::UnitHwmonitor()
{
	if(m_pHwmonitor_UnInit)
		m_pHwmonitor_UnInit();
}
UINT __stdcall CTaskServer::ReportTempratureThread(LPVOID lpVoid)
{
	CTaskServer* pThis = reinterpret_cast<CTaskServer*>(lpVoid);
	
	do
	{
		pThis->ReportTemprature();
	}while (WaitForSingleObject(ghExited, 30*1000) == WAIT_TIMEOUT);
	
	return ERROR_SUCCESS;
}
UINT __stdcall CTaskServer::PushGameThread(LPVOID lpVoid)
{
	CTaskServer* pThis = reinterpret_cast<CTaskServer*>(lpVoid);

	//游戏配置文件
	{
		if(!AfxIsSuperUser())
		{
			for (size_t i=0;i<pThis->gGameConfig.size();i++)
			{
				TCHAR key[MAX_PATH];
				_stprintf_s(key, _countof(key), TEXT("%d"), pThis->gGameConfig[i].gid);
				DWORD dwValue = pThis->gGameConfig[i].gameconfig;
				SHSetValue(HKEY_LOCAL_MACHINE, SOFTKEY_GEFFECT, key, REG_DWORD, &dwValue, sizeof(DWORD));
				if(dwValue < 100)
					continue;

				TCHAR szDstFile[MAX_PATH] = {0};
				AfxGetAppPath(szDstFile,MAX_PATH,_T("geffect\\"));
				lstrcat(szDstFile, key);lstrcat(szDstFile, _T(".cfg"));
				pThis->DownLoadFile(pThis->gGameConfig[i].szFileName,szDstFile);
			}
		}
	}

	DWORD IsDiskLess = 0;
	if (ERROR_SUCCESS == ScsiDiskQueryDisklessInfo(&IsDiskLess, NULL, NULL) && (IsDiskLess != 0))
		return ERROR_SUCCESS;

	TCHAR szDllName[MAX_PATH] = {0};
#ifndef _WIN64
	AfxGetAppPath(szDllName, _countof(szDllName), TEXT("kupdate.dll"));
#else
	AfxGetAppPath(szDllName, _countof(szDllName), TEXT("kupdate64.dll"));
#endif
	HMODULE hMod = LoadLibrary(szDllName);
	KStartUpdate = (KSTARTUPDATE)GetProcAddress(hMod, "LibUpdate_StartUpdate");
	KStopUpdate  = (KSTOPUPDATE)GetProcAddress(hMod, "LibUpdate_StopUpdate");
	KGetStatus   = (KGETSTATUS)GetProcAddress(hMod, "LibUpdate_GetStatus");
	if (hMod == NULL || KStartUpdate == NULL || KStopUpdate == NULL || KGetStatus == NULL)
	{
#ifndef _WIN64
		AfxWriteLogger(ghLogger, TEXT("load kupdate.dll fail."));
#else
		AfxWriteLogger(ghLogger, TEXT("load kupdate64.dll fail."));
#endif
		return ERROR_SUCCESS;
	}

	CEvenyOneSD sd;
	HANDLE hNotify = CreateEvent(sd.GetSA(), TRUE, FALSE, UPDATE_EVENT_NAME);
	if (hNotify == NULL)
	{
		AfxWriteLogger(ghLogger, TEXT("PushGameThread create event fail."));
		return ERROR_SUCCESS;
	}

	DWORD speed = pThis->GetSysOptInteger(OPT_D_PUSHSPEED);	
	for (std::map<DWORD, tGameInfo>::iterator it = pThis->gPushInfo.begin(); it != pThis->gPushInfo.end(); it++)
	{
		if (WaitForSingleObject(ghExited, 0) == WAIT_OBJECT_0)
			break;
		pThis->PushSingleGame(hNotify, it->second, speed);
	}
	CloseHandle(hNotify);
	if (hMod != NULL)
		FreeLibrary(hMod);
	return ERROR_SUCCESS;
}

bool __stdcall AfxCreateTaskServer()
{
	return CTaskServer::getInstance()->StartTaskServer();
}

void __stdcall AfxStopTaskServer()
{
	CTaskServer::getInstance()->StopTaskServer();
}



bool AfxIsSuperUser()
{
	DWORD IsAdmin = 0,IsDiskLess = 0;
	if (ERROR_SUCCESS == ScsiDiskQueryDisklessInfo(&IsDiskLess, &IsAdmin, NULL) && (IsAdmin != 0))
		return true;

	if(IsDiskLess)
		return false;

	HKEY  hkey = NULL;
	DWORD nValue = 0, nLenth = sizeof(DWORD);

	//1,锐起
	long nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\Richdisk"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("ClientFlag"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue > 0)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	//2,EMS
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\EMSDISK"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("Shutdown"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 0)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	//3,OBM
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\UxdScsi"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("WksMode"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 2)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	//4,网维大师
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\iCafe8"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("Admin"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 1)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}

	//5,易游
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\EYOOCLIENTSTATUS"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("SuperLogin"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 1)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	//6,信佑
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\HintRoot"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("Start"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 3)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	//7,网众
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\NXD"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("SUPER"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 1)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}

	//8,Sanboot
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\eBoot\\Context"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		char szValue[4096] ={0};
		nLenth = 4096 ;
		nResult = RegQueryValueEx(hkey, TEXT("Data"), NULL, NULL, (LPBYTE)&szValue, &nLenth);
		if (nResult == ERROR_SUCCESS && szValue[11] == 16)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}

	//9,vhd
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\VHD\\Context") ,0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		char szValue[4096] ={0};
		nLenth = 4096 ;
		nResult = RegQueryValueEx(hkey, TEXT("Data"), NULL, NULL, (LPBYTE)&szValue, &nLenth);
		if (nResult == ERROR_SUCCESS && szValue[39] == 48)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	return false;
}


DWORD   CTaskServer::GetServerIpUseBroad(bool bTakeOver)
{
	char				szAdaptersBuf[PKG_MAXLENGTH] = {0};
	PIP_ADAPTER_INFO	pAdapters = (PIP_ADAPTER_INFO)szAdaptersBuf;
	PIP_ADAPTER_INFO	pNextAdapter = NULL; 
	DWORD				dwBufSize = PKG_MAXLENGTH;

	if (bTakeOver)
	{
		AfxWriteLogger(ghLogger,_T("start broadcast query server..."));
	}
	else
	{
		AfxWriteLogger(ghLogger,_T("start broadcast query takeover..."));
	}

	if (NO_ERROR != GetAdaptersInfo(pAdapters, &dwBufSize) || dwBufSize < sizeof(IP_ADAPTER_INFO))
	{
		AfxWriteLogger(ghLogger,_T("query adapters at broadcast failed,%d"), GetLastError());
		return 0;
	}

	pNextAdapter = pAdapters;
	do
	{
		PIP_ADDR_STRING pNextIPAddress = &pNextAdapter->IpAddressList;
		do 
		{
			DWORD dwIP = inet_addr(pNextIPAddress->IpAddress.String);
			DWORD dwMask = inet_addr(pNextIPAddress->IpMask.String);
			DWORD dwBcAddr = htonl(dwIP) | ~htonl(dwMask);

			if (dwIP != 0 && dwIP != 0x100007f)
			{
				if (bTakeOver)
				{
					AfxWriteLogger(ghLogger,_T("broadcast query takeover to %x"), dwBcAddr);
				}
				else
				{
					AfxWriteLogger(ghLogger,_T("broadcast query server to %x"), dwBcAddr);
				}

				DWORD nIp;
				if (AfxDetectTakeoverEx(bTakeOver ? 0:1,dwBcAddr, (LPDWORD)&nIp,1,3))
				{
					return nIp;
				}
			}
			pNextIPAddress = pNextIPAddress->Next;
		}while (pNextIPAddress != NULL);
		pNextAdapter = pNextAdapter->Next;
	}while(pNextAdapter != NULL);

	return 0;
}
void InjectToExplorere();

void CTaskServer::UnloadRapidPowerShutdown()
{
	if (m_pDisableRapidPowerShutDown)
		m_pDisableRapidPowerShutDown();
}
void CTaskServer::LoadRapidPowerShutdown()
{
	if (1 != GetSysOptInteger(OPT_M_SHUTDOWNENABLE))
		return;

	HMODULE		hMod = NULL;
	TCHAR szDllName[MAX_PATH] = { 0 };
#ifndef _WIN64
	AfxGetAppPath(szDllName, _countof(szDllName), TEXT("kpowershut.dll"));
#else
	AfxGetAppPath(szDllName, _countof(szDllName), TEXT("kpowershut64.dll"));
#endif
	if (hMod = LoadLibrary(szDllName))
	{
		m_pEnableRapidPowerShutDown = (DISABLERAPIDPOWERSHUTDOWN)GetProcAddress(hMod, "EnableRapidPowerShutdown");
		m_pDisableRapidPowerShutDown = (ENABLERAPIDPOWERSHUTDOWN)GetProcAddress(hMod, "DisableRapidPowerShutdown");
		
		if (m_pEnableRapidPowerShutDown)
		{
			m_pEnableRapidPowerShutDown();
			AfxWriteLogger(ghLogger, _T("load rapid power shut down sucess."));
		}
			
	}
}

BOOL CTaskServer::FixPubwin()
{
	BOOL bResult = FALSE;

	PROCESSENTRY32	pe32      = {0};
	HANDLE			hProcessSnap = NULL;  

	pe32.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		AfxWriteLogger(ghLogger,_T("createtoolhelp32snapshot all process faild:%d."),GetLastError());
		return bResult;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(hProcessSnap, &pe32))
	{
		AfxWriteLogger(ghLogger,_T("process32first jb faild:%d."),GetLastError());
		CloseHandle(hProcessSnap);
		return bResult;
	}

	do 
	{
		unsigned char pChar[] = {0x5f,0x6d,0x11,0xde,0x1e,0x93,0x52,0xc1,0x5b,0x85,0x07,0xaf,0xfd,0xf3,0xa2,0x7b};
		char szExe[MAX_PATH] = {0};
		AfxDecryptVnc(pChar,szExe,sizeof(pChar));

		if(_tcsicmp(pe32.szExeFile,(_bstr_t)szExe) != 0)
			continue;

		HANDLE	hProcess = OpenProcess(PROCESS_ALL_ACCESS,0,pe32.th32ProcessID);
		if(hProcess == INVALID_HANDLE_VALUE)
		{
			AfxWriteLogger(ghLogger,_T("openprocess jb faild:%d."),GetLastError());
			break;
		}
		HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
		if(hModuleSnap == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hProcess);
			AfxWriteLogger(ghLogger,_T("createtoolhelp32snapshot jb faild:%d."),GetLastError());
			break;
		}

		MODULEENTRY32 me32 = {0};
		me32.dwSize = sizeof(MODULEENTRY32);
		BOOL bModeFirst = Module32First(hModuleSnap, &me32);
		if(!bModeFirst)
		{
			CloseHandle(hProcess);
			CloseHandle(hModuleSnap);
			AfxWriteLogger(ghLogger,_T("module32first jb faild:%d."),GetLastError());
			break;
		}

		SIZE_T	retnlen = 0;
		BYTE	tmp[0x100] = {0};
		//获取jb进程需要修改的内存
		if(!ReadProcessMemory(hProcess,(PVOID)((DWORD)me32.hModule+0x6E490),tmp,sizeof(tmp),&retnlen) ||
			retnlen <= 0x25 )
		{
			CloseHandle(hProcess);
			CloseHandle(hModuleSnap);
			AfxWriteLogger(ghLogger,_T("ReadProcessMemory jb faild:%d."),GetLastError());
			break;
		}

		//判断是否符合需要修改的版本
		if( *(DWORD *)tmp == 0xA164 && *(DWORD *)(tmp+0x4) == 0xFF6A0000 && *(BYTE *)(tmp+0xD) == 0x50 && *(DWORD *)(tmp+0x1F) == 0xC2840F )
		{
			DWORD retnlenex = 0;
			BYTE jmpcode[] = {0xE9,0xC3,0x00,0x00,0x00,0x05,0x38,0xD5,0xA5,0xF7};
			VirtualProtectEx(hProcess, (PVOID)((DWORD)me32.hModule + 0x6E4AF), sizeof(jmpcode), PAGE_EXECUTE_READWRITE, &retnlenex);
			//修改jb进程
			if( !WriteProcessMemory(hProcess,(PVOID)((DWORD)me32.hModule+0x6E4AF),jmpcode,sizeof(jmpcode),&retnlen) )
			{
				AfxWriteLogger(ghLogger,_T("fix jb faild:%d."),GetLastError());
			}
			else
			{
				bResult = TRUE;
				AfxWriteLogger(ghLogger,_T("fix jb success."));
			}
		}
		//判断是自己修改的jb不进行其他操作
		else if( *(DWORD *)tmp == 0xA164 && *(DWORD *)(tmp+0x4) == 0xFF6A0000 && *(BYTE *)(tmp+0xD) == 0x50 && *(DWORD *)(tmp+0x1F) == 0xC3E9 )
		{
			bResult = TRUE;
			AfxWriteLogger(ghLogger,_T("already fix jb."));
		}
		//特征匹配失败，提示！
		else
		{
			bResult = TRUE;
			AfxWriteLogger(ghLogger,_T("not match jb version."));
		}

		CloseHandle(hProcess);
		CloseHandle(hModuleSnap);
		break;

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return bResult;
}
std::wstring AfxUTF82Unicode(LPCSTR  pUTF8)
{
	int nLen = MultiByteToWideChar(CP_UTF8, 0, pUTF8, -1, NULL, 0);
	std::wstring result;
	result.resize(nLen);
	MultiByteToWideChar(CP_UTF8, 0, pUTF8, -1, &result[0], nLen);
	if (result.size())
		result.resize(result.size()-1);
	return result;
}
void CTaskServer::ParerIdcPushXml()
{
	std::vector<tagShortCutIconInfo> arShortCuts;
	std::vector<tagWallPaper>  arWallPaper;

	bool bResult = false;
	TCHAR szPath[MAX_PATH] = {0};
	AfxGetAppPath(szPath,MAX_PATH,_T("config\\pushnetbar.xml"));
	if(DownLoadFile(TEXT("data\\pushnetbar.xml"), szPath, -1, -1, TRUE))
	{
		try
		{
			MSXML2::IXMLDOMDocumentPtr xmlDoc(_uuidof(MSXML2::DOMDocument)); 
			if (VARIANT_TRUE == xmlDoc->load((_bstr_t)szPath))
			{
				MSXML2::IXMLDOMElementPtr XMLROOT;
				XMLROOT = xmlDoc->GetdocumentElement();
				MSXML2::IXMLDOMNodePtr nodeList  = XMLROOT->selectSingleNode(_T("/doc/shortcuts"));
				MSXML2::IXMLDOMNodeListPtr ptr = nodeList->GetchildNodes();
				for (long idx=0; idx<ptr->length; idx++)
				{
					tagShortCutIconInfo tagShort = {0};
					MSXML2::IXMLDOMElementPtr node  = ptr->Getitem(idx);
					tstring str = (TCHAR*)(_bstr_t)node->getAttribute(_T("type"));
					if(_ttoi(str.c_str()) == 0)
					{
						tagShort.nType = 0;
						str = (TCHAR*)(_bstr_t)node->getAttribute(_T("gid"));
						tagShort.gid = _ttoi(str.c_str());

					}
					else
					{
						tagShort.nType = 1;
						str = (TCHAR*)(_bstr_t)node->getAttribute(_T("name"));
						_tcscpy_s(tagShort.szName,str.c_str());
						str = (TCHAR*)(_bstr_t)node->getAttribute(_T("url"));
						_tcscpy_s(tagShort.szUrl,str.c_str());
						str = (TCHAR*)(_bstr_t)node->getAttribute(_T("icon_url"));
						_tcscpy_s(tagShort.szIconUrl,str.c_str());
						tstring strIcoUrl = str;
						size_t nPos = strIcoUrl.rfind(_T('/'));
						if(nPos != tstring::npos)
						{
							tstring strIconfile = strIcoUrl.substr(nPos+1,strIcoUrl.length()-nPos);
							_tcscpy_s(tagShort.szFileName,strIconfile.c_str());
						}

					}
					arShortCuts.push_back(tagShort);
				}

				nodeList  = XMLROOT->selectSingleNode(_T("/doc/wallpapers"));
				ptr = nodeList->GetchildNodes();
				for (long idx=0; idx<ptr->length; idx++)
				{
					tagWallPaper tagWall = {0};
					MSXML2::IXMLDOMElementPtr node  = ptr->Getitem(idx);
					tstring str = (TCHAR*)(_bstr_t)node->getAttribute(_T("name"));
					_tcscpy_s(tagWall.szName,str.c_str());

					str = (TCHAR*)(_bstr_t)node->getAttribute(_T("pic_url"));
					_tcscpy_s(tagWall.szPicUrl,str.c_str());
					tstring strIcoUrl = str;
					size_t nPos = strIcoUrl.rfind(_T('/'));
					if(nPos != tstring::npos)
					{
						tstring strIconfile = strIcoUrl.substr(nPos+1,strIcoUrl.length()-nPos);
						_tcscpy_s(tagWall.szFileName,strIconfile.c_str());
					}
					arWallPaper.push_back(tagWall);
				}
				bResult = true;
			}
		}
		catch(_com_error& Error)
		{
			bResult = false;
			AfxWriteLogger(ghLogger,_T("Parser netbar push xml Error:%s"),(LPCTSTR)Error.Description());
		}

	}
	if (bResult)
		CreateIdcPush(arShortCuts,arWallPaper);
}

void CTaskServer::CreateIdcPush( std::vector<tagShortCutIconInfo>& arShortCuts,std::vector<tagWallPaper>& arWallPaper )
{
	bool bRefresh = false;
	TCHAR szRootPath[MAX_PATH] = {0};
	AfxGetAppPath(szRootPath);
	for (size_t i=0;i<arShortCuts.size();i++)
	{
		if(arShortCuts[i].nType == 0)
		{
			std::map<DWORD, tGameInfo>::iterator it = gGameInfo.find(arShortCuts[i].gid);
			if(it != gGameInfo.end())
			{
				DWORD gid = it->first;
				TCHAR szExePath[MAX_PATH] = {0},szIconFile[MAX_PATH] = {0},szMemo[MAX_PATH]={0},szParam[MAX_PATH]={0};
				AfxGetAppPath(szExePath, _countof(szExePath), TEXT("knbmenu.exe"));
				_stprintf_s(szParam, _countof(szParam), TEXT("/gid=%d"), gid);
				_stprintf_s(szIconFile, _countof(szIconFile), TEXT("%s%s%d.ico"), szRootPath, IMAGE_DIR, gid);
				_stprintf_s(szMemo, _countof(szMemo), TEXT("sig=%s"), it->second.gname);
				if (DownGameDesktopIco(gid, szIconFile) || ExactIconFromDb(gid, szIconFile))
				{
					CreateDesktopLink(it->second.gname, szExePath, szParam, szIconFile, szMemo, bRefresh);
					m_LinkList.insert(std::make_pair(szMemo, it->second.gname));
				}
				else
					AfxWriteLogger(ghLogger, TEXT("create push game shortcut fail,ExcatIcon %d fail."), gid);
			}
			else
			{
				AfxWriteLogger(ghLogger, TEXT("not find game,create push game shortcut fail:%d"), arShortCuts[i].gid);
			}

		}
		else
		{
			TCHAR szExePath[MAX_PATH] = {0},szIconFile[MAX_PATH] = {0},szMemo[MAX_PATH]={0},szParam[MAX_PATH]={0};
			_tcscpy_s(szExePath, arShortCuts[i].szUrl);
			_stprintf_s(szIconFile, _countof(szIconFile), TEXT("%s%s%s"), szRootPath, IMAGE_DIR, arShortCuts[i].szFileName);
			_stprintf_s(szMemo, _countof(szMemo), TEXT("sig=%s"), arShortCuts[i].szName);
			TCHAR szSrcFile[MAX_PATH] = _T("data\\");
			lstrcat(szSrcFile, arShortCuts[i].szFileName);
			if (DownLoadFile(szSrcFile, szIconFile, -1, -1))
			{
				CreateDesktopLink(arShortCuts[i].szName, szExePath, szParam, szIconFile, szMemo, bRefresh);
				m_LinkList.insert(std::make_pair(szMemo, arShortCuts[i].szName));
			}
			else
				AfxWriteLogger(ghLogger, TEXT("create push user-defined shortcut fail,ExcatIcon %s fail."), szIconFile);
		}
	}
	if (bRefresh)
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST|SHCNF_FLUSH, 0, 0);

	if(arWallPaper.size() > 0)
	{
		DWORD dwSize = (DWORD)arWallPaper.size();
		srand(_time32(NULL));
		int nIndex =  dwSize ? (rand() % dwSize) : 0;
		for (DWORD loop=0; loop<dwSize; ++loop)
		{
			if (nIndex == loop)
			{
				TCHAR szFileName[MAX_PATH] = {0}, szImage[MAX_PATH] = {0};
				_tcscpy_s(szFileName,arWallPaper[loop].szFileName);
				AfxGetAppPath(szImage, MAX_PATH, IMAGE_DIR);
				lstrcat(szImage, szFileName);
				TCHAR szSrcFile[MAX_PATH] = _T("data\\");
				lstrcat(szSrcFile, szFileName);
				if (!DownLoadFile(szSrcFile, szImage, -1, -1))
				{
					AfxWriteLogger(ghLogger, TEXT("down push wallpaper file fail:%s"), szImage);
					nIndex ++;
					continue;
				}

				TCHAR szExe[MAX_PATH] = {0}, szParameter[MAX_PATH] = {0};
				AfxGetAppPath(szExe, MAX_PATH, TEXT("kdesk.exe"));
				_stprintf_s(szParameter, _countof(szParameter), TEXT("\"/SetWallPaper=%s\""), szImage);
				AfxRunProcess(szExe, szParameter);
				AfxKillProcess(szExe);
				AfxWriteLogger(ghLogger, TEXT("set wallpaper:%s"), szImage);
				m_bPushWallpaper = TRUE;
				break;
			}
		}
	}
}

BOOL CTaskServer::LoadSafeDll(LPCTSTR ip, int port)
{
	typedef void ( _stdcall *MySafeCenterStart)(LPCTSTR ip, int port);

	BOOL				bResult = FALSE;
	MySafeCenterStart	pSafeStart = NULL;
	HMODULE				hModule = LoadLibrary(L"kscdrv.dll");

	if(hModule)
	{
		pSafeStart = (MySafeCenterStart)GetProcAddress(hModule, "SafeCenterStart");
		if (pSafeStart != NULL)
		{
			pSafeStart(ip, port);
			AfxWriteLogger(ghLogger, TEXT("Load safecenter success"));
			bResult = TRUE;
		}
	}
	else
	{
		AfxWriteLogger(ghLogger, TEXT("Load safecenter  failed:%d"), GetLastError());
	}

	return bResult;
}
void __stdcall Task_log_handler_t(void *pLogHandle, LPCWSTR msg,unsigned __int32 msg_len)
{
	AfxWriteLogger(pLogHandle, msg);
}
void __stdcall Ssd_Game_Disk(void *handle,wchar_t szSsdDriver)
{
	DWORD nDisk = (DWORD)szSsdDriver;
	SHSetValue(HKEY_LOCAL_MACHINE, SOFTWARE_KEY, _T("ssdgamedisk"), REG_DWORD, &nDisk, sizeof(DWORD));
}
void CTaskServer::GetSsdWorkMode()
{
	if (AfxIsSuperUser())
		return;
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETSSDWORKMODE, OBJECT_CLIENT);
		pkgSend << gMachine;
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw (int)'e';
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw (int)'w';
		DWORD nSsdWork =0,nIniData =0;
		pkgRecv >> nSsdWork >> nIniData;

		HMODULE		hMod = NULL;
		TCHAR szDllName[MAX_PATH] = {0};
		AfxGetAppPath(szDllName, _countof(szDllName), TEXT("kssdeploy.dll"));
		if (hMod = LoadLibrary(szDllName))
		{
			m_pCreate_Ssd = (CREATE_SSD)GetProcAddress(hMod, "create_ssd_handle");
			m_pDestroy_Ssd = (DESTROY_SSD)GetProcAddress(hMod, "destroy_ssd_handle");
			m_pStart_Ssd = (START_SSD)GetProcAddress(hMod, "start_ssd");
			m_pStop_Ssd = (STOP_SSD)GetProcAddress(hMod, "stop_ssd");
			if(m_pCreate_Ssd != NULL && m_pDestroy_Ssd != NULL &&
				m_pStart_Ssd != NULL && m_pStop_Ssd != NULL)
			{

				DISKLESS_INFO diskless_info;
				memset(&diskless_info, 0, sizeof(diskless_info));
				ScsiDiskQueryDisklessInfoEx(&diskless_info);
				m_bDeploy = diskless_info.diskless && nSsdWork?true:false;

				m_hSsdLogger = AfxCreateLogger(_T("kssd"));
				AfxWriteLogger(m_hSsdLogger, TEXT("=============================================="));
				callback_t callback = {0};
				callback.log_handler_ = Task_log_handler_t;
				callback.ssd_game_disk_handler_ = Ssd_Game_Disk;
				m_pSsdHandle = m_pCreate_Ssd(callback,m_hSsdLogger);
				if(m_pSsdHandle != NULL)
				{
					param_t para = {0};
					para.is_deployment_ = m_bDeploy;
					para.force_init_type = nIniData;
					para.is_create_process_ = m_bDeploy;
					para.is_deploy_mode_need_update_game_ = GetSysOptInteger(OPT_S_DEPLOY_SYNGAME,1) ? true:false;
					para.is_none_deploy_mode_need_update_game_ = GetSysOptInteger(OPT_S_NODEPLOY_SYNGAME,1) ? true:false;
					para.none_deploy_mode_limit_update_speed_ = GetSysOptInteger(OPT_S_SSDSYNSPEED,10485760);
					para.writecallback_disk_size_ = GetSysOptInteger(OPT_S_WRITESIZE,10);
					para.is_check_vendor_ = GetSysOptInteger(OPT_U_ISSSD,0) ? true:false;
					if(m_pStart_Ssd(m_pSsdHandle,para))
						AfxWriteLogger(ghLogger, TEXT("start_ssd success."));
				}
				else
					AfxWriteLogger(ghLogger, TEXT("create_ssd_handle faild."));
			}
		}
		AfxWriteLogger(ghLogger, TEXT("query ssd work mode success."));
	}
	catch (int &e)
	{
		AfxWriteLogger(ghLogger, TEXT("GetSsdWorkMode fail %d."),e);
	}
	catch (...)
	{
		AfxWriteLogger(ghLogger, TEXT("query ssd work mode  fail."));
	}
}

void CTaskServer::DownLoadResZip()
{
	TCHAR szPath[MAX_PATH] = {0};
	AfxGetAppPath(szPath,MAX_PATH,_T("config\\res.zip"));
	DownLoadFile(TEXT("data\\res.zip"), szPath, -1, -1, TRUE);
}
