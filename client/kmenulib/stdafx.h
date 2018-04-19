// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的
#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS         // 移除对话框中的 MFC 控件支持

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            //  从 Windows 头文件中排除极少使用的信息
#endif

#include <afx.h>
#include <afxwin.h>         // MFC 核心组件和标准组件

#include <GdiPlus.h>


// TODO:  在此处引用程序需要的其他头文件

#define  FROM_CLICK_GAME		"1"
#define  FROM_CLICK_AD			"2"
#define  FROM_CLICK_NAV			"3"
#define  FROM_CLICK_TOOL		"4"
#define  FROM_CLICK_LOGIN		"5"


#define  POS_MENU				"s1"
#define  POS_DESK				"s2"
#define  POS_RUNGAME			"s3"
#define  POS_GAMEPRI			"s4"
#define  POS_MENUSEARDROP		"s5"
#define  POS_MENUSEARCH			"s6"
#define  POS_AD1				"s7"
#define  POS_AD2				"s8"
#define  POS_AD3				"s9"
#define  POS_AD4				"s10"
#define  POS_AD5				"s11"


#define  TOOL_SKIN				"t1"
#define  TOOL_SAVESKIN			"t2"
#define  TOOL_OPENUDISK			"t3"
#define  TOOL_LOGINUDISK		"t4"
#define  TOOL_OPENPROFILE		"t5"
#define  TOOL_SAVEPROFILE		"t6"
#define  TOOL_READPROFILE		"t7"
#define  TOOL_LOCK				"t8"
#define  TOOL_KEYBOARD			"t9"
#define  TOOL_MOUSE				"t10"
#define  TOOL_VOICE				"t11"
#define  TOOL_INPUTMETHOD		"t12"
#define  TOOL_LOGIN				"t13"
#define  TOOL_REGISTER			"t14"
#define  TOOL_HEAD				"t15"
#define  TOOL_SEARCHGAME		"t16"

#define CLIENT_INII_SECTION_SERVER	TEXT("config")
#define CLIENT_INII_KEY_IP			TEXT("serverip")

#define CLIENT_DATA_SYSOPT			TEXT("config\\koption.dat")
#define CLIENT_DATA_VDISK			TEXT("config\\kvdisk.dat")
#define CLIENT_DATA_GAME			TEXT("config\\kgame.dat")
#define CLIENT_DATA_GAMEICON		TEXT("config\\kicons.dat")
#define CLIENT_DATA_SERVERCFG		TEXT("config\\ksvrlist.dat")
#define CLIENT_DATA_TASKLIST		TEXT("config\\tasklist.dat")
#define CLIENT_DATA_RATE			TEXT("config\\krate.dat")

#define MAX_NAME_LEN				64
#define MAX_OPTION_LEN				1024
#define CLI_SERVICE_NAME			TEXT("knbclient")
#define CLI_EXE_NAME				TEXT("knbclient.exe")

#define INDEX_FILE_NAME				TEXT("package.pkg")

#define FRAME_CLASSNAME				TEXT("{1507D6FF-4F39-46d6-8406-6A98F3B5FFFD}")

#define MENU_SEARCH_CLICK_ID				99

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
	DWORD SvrVer;
	DWORD gsize;
	DWORD gtoolbar;
	DWORD gpush;
	DWORD desklnk;
	DWORD toolbar;
	DWORD svrclick;
	DWORD invalue;
	DWORD runtype;
	DWORD hottag;
	DWORD ideclick;
	DWORD idcdate;
	DWORD iWait;
}tGameInfo;

typedef struct tagGameInfo : public tGameInfo
{
	int			dwFindPos;
	bool        bGalias;
	TCHAR		szPy[MAX_PATH];
	TCHAR		szMultiPy[MAX_PATH];
}tagGameInfo;

typedef struct tagIconInfo
{
	DWORD dwGid;
	DWORD dwHash;
	DWORD dwLength;
	char* pIcon;
	Gdiplus::Bitmap* pImage;
}tagIconInfo;

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

typedef struct tServer
{
	TCHAR szServer[MAX_NAME_LEN];
	DWORD dwIp;
}tServer;

typedef struct tRate
{
	TCHAR szTitle[MAX_NAME_LEN];
	DWORD nStartIp;
	DWORD nEndIp;
	TCHAR szContent[MAX_PATH];
}tRate;

