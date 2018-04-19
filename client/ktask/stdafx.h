#pragma once

#ifndef WINVER
#define WINVER 0x0600
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0600
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#define WIN32_LEAN_AND_MEAN

#include "kcore.h"
#include <windows.h>
#include <stdlib.h>
#include <comutil.h>
#include <shlwapi.h>
#include <WinInet.h>
#include <ShlObj.h>
#include <process.h>
#include <atlstr.h>

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "comsuppw")

#ifndef _WIN64
#pragma comment(lib, "kcore")
#else
#pragma comment(lib, "kcore64")
#endif

extern HANDLE ghLogger;
extern HANDLE ghExited;
extern SOCKET gSocket;
extern HANDLE ghModule;

USE_NAMESPACE_KCORE


typedef struct tGameInfo
{
	DWORD gid;
	TCHAR Class[MAX_PATH];
	TCHAR cid[MAX_PATH];
	TCHAR gname[MAX_PATH];
	TCHAR galias[MAX_PATH];
	TCHAR grungame[MAX_PATH];
	TCHAR gexefile[MAX_PATH];
	TCHAR gparameter[MAX_PATH];
	TCHAR gsvrpath[MAX_PATH];
	TCHAR gclipath[MAX_PATH];
	TCHAR gprogress[MAX_PATH];
	TCHAR gConfig[MAX_PATH];
	DWORD gsize;
	DWORD gtoolbar;
	DWORD gpush;
	DWORD desklnk;
	DWORD toolbar;
	DWORD svrver;
	DWORD svrclick;
	DWORD invalue;
	DWORD runtype;
	DWORD hottag;
	DWORD Idcclick;
	DWORD idcdate;
	DWORD nWait;
}tGameInfo;

typedef struct tagIconInfo
{
	DWORD dwGid;
	DWORD dwHash;
	DWORD dwLength;
	char* pIcon;
}tagIconInfo;

typedef struct tClientInfo
{
	TCHAR Machine[MAX_PATH];
	TCHAR Mac[MAX_PATH];
	DWORD Ip;
	DWORD Mark;
	DWORD Gate;
	DWORD DNS;
	DWORD DNS2;
	DWORD NetSpeed;
	DWORD ArpRunStatus;
	TCHAR AllArea[MAX_PATH];
	TCHAR ProtArea[MAX_PATH];
	TCHAR Version[MAX_PATH];

	TCHAR OS[MAX_PATH];
	TCHAR Cpu[MAX_PATH];
	TCHAR MainBoard[MAX_PATH];
	TCHAR Memory[MAX_PATH];
	TCHAR Disk[MAX_PATH];
	TCHAR Video[MAX_PATH];
	TCHAR NetCard[MAX_PATH];
	TCHAR DiskId[MAX_PATH];
	TCHAR VideoId[MAX_PATH];
	TCHAR NetCardId[MAX_PATH];
	TCHAR Camera[MAX_PATH];
	TCHAR Explorer[MAX_PATH];
	TCHAR JHSoft[MAX_PATH];
	TCHAR WPSoft[MAX_PATH];
}tClientInfo;

typedef struct tDiskMgr
{
	TCHAR SERVER[MAX_PATH];
	DWORD DiskDriver;
	DWORD DiskType;
	DWORD VdiskLetter;
	DWORD MemCache;
	DWORD SSDCache;
	DWORD CachePageSize;
	DWORD SSDDriver;
}tDiskMgr;

typedef struct tGameConfig
{
	DWORD gid;
	DWORD gameconfig;
	TCHAR szFileName[MAX_PATH];
}tGameConfig;
struct tagShortCutIconInfo
{
	DWORD		gid;
	DWORD       nType;
	DWORD		nFileModifyTime;
	TCHAR		szName[MAX_PATH];
	TCHAR		szUrl[MAX_PATH];
	TCHAR		szIconUrl[MAX_PATH];
	TCHAR		szFileName[MAX_PATH];
};

struct tagWallPaper
{
	TCHAR		szName[MAX_PATH];
	TCHAR		szPicUrl[MAX_PATH];
	TCHAR		szFileName[MAX_PATH];
};
struct tagGameShortCutItem
{
	DWORD gId;
	TCHAR gname[MAX_PATH];
	TCHAR szExePath[MAX_PATH];
	TCHAR szParam[MAX_PATH];
	TCHAR szIconFile[MAX_PATH];
	TCHAR szMemo[MAX_PATH];
};

struct tagSystemShortCutItem
{
	DWORD dwType;
	DWORD gId;
	TCHAR szName[MAX_PATH];
	TCHAR szExePath[MAX_PATH];
	TCHAR szParam[MAX_PATH];
	TCHAR szIconPath[MAX_PATH];
	TCHAR szIconFile[MAX_PATH];
	TCHAR szMemo[MAX_PATH];
};

struct tagServerList
{
	DWORD Id;							// [tServer].[ROWID]
	DWORD ip;							// [ip]
	DWORD flag;							// 
};
struct tagAllServerGameDisk
{
	DWORD ip;							// [tIpMgr].[ip]
	DWORD diskdriver;					// [diskdriver]
	DWORD vdiskletter;					// [vdiskletter]
};
//ÔöÖµ
#define  VALUE_ADD_FILE_MAP_NAME			TEXT("Global\\__value_add_map__")

#define ADDVALUE_SHARE_SIG	0x43474d41
#define MAX_AGENTID_SIZE	32
#define MAX_NETBARID_SIZE	32
#define MAX_MAC_SIZE		32

typedef struct
{
	DWORD dwSignature;
	char szAgentId[MAX_AGENTID_SIZE];
	char szNetbarId[MAX_NETBARID_SIZE];
	char szMac[MAX_MAC_SIZE];
	char szReserved[128];
}ADDVALUE_SHARE_DATA, *PADDVALUE_SHARE_DATA;


bool __stdcall AfxCreateTaskServer();
void __stdcall AfxStopTaskServer();

bool AfxExecuteSoftUpdate(bool& bNeedReboot);
bool AfxIsSuperUser();

inline DWORD  __stdcall AfxSocketExecuteLock(SOCKET& sck, LPCSTR pOutData, int outLength, LPSTR pInData, int inLength)
{
	static CLock lock;
	OBJECT_LOCK(lock);
	DWORD dwRet = AfxSocketExecute(sck, pOutData, outLength, pInData, inLength);
	if (dwRet != ERROR_SUCCESS)
		AfxSocketClose(sck);
	return dwRet;
}

#define MASK_WINXP_RUN		(1 << 1)
#define MASK_WIN7_RUN		(1 << 2)
#define MASK_WIN10_RUN		(1 << 3)

typedef enum {
	OS_UNKNOWN,
	OS_WIN95,
	OS_WIN98,
	OS_WINME,
	OS_NT351,
	OS_NT4,
	OS_WIN2000,
	OS_WINXP,
	OS_VISTA,
	OS_WIN7,
	OS_WIN8,
	OS_WIN10,
	OS_NEWEST,
}OS_VER;

bool AfxGetOSVerEx(OS_VER& os, RTL_OSVERSIONINFOEXW& osinfo);
OS_VER AfxGetOSVer();

#define CLIENT_DATA_DIALOG			TEXT("config\\kdeskdialog.dat")
#define MAX_NAME_LEN				64
typedef struct
{
	int nType;
	int x;
	int y;
	int nWith;
	int nHeight;
	int nTop;
	int nBotom;
}DIALOG_INFO,*PDIALOG_INFO;

typedef struct ItemShortCut
{
	TCHAR strName[MAX_NAME_LEN];
	TCHAR strWorkDir[MAX_PATH];
	TCHAR strExe[MAX_PATH];
	TCHAR strParam[MAX_PATH];
	TCHAR strIconPath[MAX_PATH];
	TCHAR strMemo[MAX_PATH];
}ItemShortCut;
