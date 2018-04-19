#pragma once

#include "ntfspublic.h"

#include <map>
#include <set>
#include <tchar.h>

START_NAMESPACE_NTFS

typedef struct tDriverInfo
{
	TCHAR OSFlag[MAX_PATH];			// 5.1_x86; 6.1_x86, 6.1_x64
	TCHAR DeviceId[MAX_PATH];
	TCHAR SvrName[MAX_PATH];
	TCHAR InfSection[MAX_PATH];
	TCHAR DeviceMfg[MAX_PATH];
	TCHAR DeviceProvider[MAX_PATH];
	TCHAR DeviceDesc[MAX_PATH];
	TCHAR DriverVer[MAX_PATH];
	TCHAR DriverDate[MAX_PATH];
	tDriverInfo()
	{
		::memset(this, 0, sizeof(tDriverInfo));
	}
} TDRIVERINFO;

#define OS_51_X86	TEXT("5.1_x86")
#define OS_61_X86	TEXT("6.1_x86")
#define OS_61_X64	TEXT("6.1_x64")
#define OS_10_X86	TEXT("10.0_x86")
#define OS_10_X64	TEXT("10.0_x64")

typedef struct tagSetKeyCompare
{
	bool operator()(const ntfs_tstring &key1, const ntfs_tstring &key2) const
	{
		return ::_tcsicmp(key1.c_str(), key2.c_str()) < 0;
	}
} SETKEYCOMPARE;

typedef std::set<libntfs::ntfs_tstring, SETKEYCOMPARE> SFILEINFO;
typedef SFILEINFO::iterator SFILEINFOITR;

/// INF文件中生成REG
bool GeneratePnpRegFromInf(LPCTSTR lpszInfFile, tDriverInfo &DrvInfo, SFILEINFO &sFileInfo,
						   LPCTSTR lpszSaveRegFile = NULL, LPTSTR lpszError = NULL, DWORD dwLen = 0);

typedef struct tagDeviceInfo
{
	WORD	wVenorId;
	WORD	wDeviceId;
	DWORD	dwSubSys;
	WORD	wRev;
	tagDeviceInfo()
		: wVenorId(-1)
		, wDeviceId(-1)
		, dwSubSys(-1)
		, wRev(-1)
	{

	}
} DEVICEINFO;

/// 获取INF所有设备ID
bool GetAllDeviceInfoFormInf(LPCTSTR lpszInfFile, tDriverInfo &DrvInfo, std::vector<tagDeviceInfo> &tDeviceInfo,
							 LPTSTR lpszError = NULL, DWORD dwLen = 0);

typedef struct tagRegMapKeyCompare
{
	bool operator()(const ntfs_tstring &key1, const ntfs_tstring &key2) const
	{
		return ::_tcsicmp(key1.c_str(), key2.c_str()) < 0;
	}
} REGMAPKEYCOMPARE;
typedef struct tagRegKeyValue
{
	DWORD dwType;
	void* pValue;
	DWORD dwSize;
} REGKEYVALUE;
typedef std::map<ntfs_tstring, tagRegKeyValue, REGMAPKEYCOMPARE> REGKEYVALUEINFO;
typedef REGKEYVALUEINFO::iterator REGKEYVALUEINFOITR;
typedef std::map<ntfs_tstring, REGKEYVALUEINFO, REGMAPKEYCOMPARE> REGKEYINFO;
typedef REGKEYINFO::iterator REGKEYINFOITR;

/// 从REG文件中获取所有键值
/// 注意：mRegKeyInfo使用后需调用ReleaseRegKeyInfo，否则会内存泄露
bool GetAllKeyInfoFormReg(LPCTSTR lpszRegFile, REGKEYINFO& mRegKeyInfo, HANDLE hEvent = NULL,
						  LPTSTR lpszError = NULL, DWORD dwLen = 0);

/// 从REG文件中网卡高级键值
/// 注意：mClassRegKeyInfo,mParamsRegKeyInfo,menumRegKeyInfo 使用后需调用ReleaseRegKeyInfo，否则会内存泄露
bool GetAllNetParamInfoFormReg(LPCTSTR lpszRegFile, LPCTSTR lpszDeviceID, 
							   REGKEYINFO& mRegKeyInfo, REGKEYINFO& mClassRegKeyInfo,
							   REGKEYINFO& mParamsRegKeyInfo,
							   REGKEYINFO& menumRegKeyInfo, HANDLE hEvent = NULL,
							   LPTSTR lpszError = NULL, DWORD dwLen = 0);

/// 保存到REG文件中
/// 注意：mRegKeyInfo使用后需调用ReleaseRegKeyInfo，否则会内存泄露
bool SaveAllKeyInfoToFile(LPCTSTR lpszRegFile, REGKEYINFO& mRegKeyInfo, LPTSTR lpszError = NULL, DWORD dwLen = 0);

/// 向结构中添加某一项
bool AddRegToInfo(REGKEYINFO& mRegKeyInfo, LPCTSTR lpszKeyPath, LPCTSTR lpszValueName = NULL, 
				  DWORD dwType = REG_NONE, const void* lpszValue= NULL, DWORD dwSize = 0);

/// 向结构中删除某一项
bool RemoveRegFromInfo(REGKEYINFO& mRegKeyInfo, LPCTSTR lpszKeyPath, LPCTSTR lpszValueName = NULL, 
					DWORD dwType = REG_NONE, const void* lpszValue= NULL, DWORD dwSize = 0);

/// 释放键值信息
void ReleaseRegKeyInfo(REGKEYINFO& mRegKeyInfo);

END_NAMESPACE_NTFS