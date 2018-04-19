#pragma once

#ifndef LIBXCACHE_EXPORTS
#ifdef _WIN64
#pragma comment(lib, "kcache64")
#else
#pragma comment(lib, "kcache")
#endif 
#endif

#define FUN_DEVICE_NAME			L"\\Device\\xcache"
#define FUN_DOS_NAME			L"\\DosDevices\\xcache"
#define FUN_APP_NAME			L"\\\\.\\xcache"
#define SERVICE_NAME			L"kcache"

#define IOCTL_START_XCACHE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 开启缓存
#define IOCTL_STOP_XCACHE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 停止缓存
#define IOCTL_QUERY_XCACHE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 查询缓存
#define IOCTL_SETADD_XCACHE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 设置需要数据到缓存
#define IOCTL_CHECK_XCACHE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x905, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 检查是否外部缓存
#define IOCTL_UPDATE_XCACHE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x906, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 更新缓存
#define IOCTL_GETBLOCKNUM_XCACHE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x907, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 获取缓存链表块数量
#define IOCTL_GETBLOCKNUMX_XCACHE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x908, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 获取缓存链表块数量
#define IOCTL_GETBLOCKLIST_XCACHE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x909, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 获取缓存链表块数据

// ===================================== old device ===============================================
#define FUN_DEVICE_OLDNAME		L"\\Device\\KCache"
#define FUN_DOS_OLDNAME			L"\\DosDevices\\KCache"
#define FUN_APP_OLDNAME			L"\\\\.\\KCache"

#define FILE_DEVICE_YCACHE		0x7676
// =================================================================================================

#pragma pack(push, 1)

typedef struct tagDriverLetterInfo
{
	WCHAR wDriverLetter;			// 盘符
	ULONGLONG ullDriverSize;		// 大小(扇区数)
	BOOLEAN bEnable;				// 开启缓存
} DRIVERLETTERINFO, *PDRIVERLETTERINFO;
typedef struct tagStartCacheParam
{
	ULONG ulIndexCacheInfoId;		// 缓存ID
	ULONG ulCacheSize;				// 总大小(MB)
	ULONG ulBlockSize;				// 单块大小(KB)
	WCHAR wSSDDriverLetter;			// 盘符
	ULONG ulSSDCacheSize;			// 总大小(MB)
	ULONG ulCount;					// 盘符个数
	DRIVERLETTERINFO wDriverInfo[1];// 盘符信息
} STARTCACHEPARAM, *PSTARTCACHEPARAM;

typedef struct  tagDiskStatistics
{
	WCHAR		wDriverLetter;		// 盘符(大写)
	ULONG		ulUserCount;		// 块使用个数
	ULONGLONG	ullAllReadSize;		// 总读取
	ULONGLONG	ullCacheReadSize;	// 缓存读取
	ULONGLONG	ullAllWriteSize;	// 总写入
	ULONGLONG	ullCacheWriteSize;	// 缓存写入
} DISKSTATISTICS, *PDISKSTATISTICS;
typedef struct tagCacheStatistics
{
	ULONG		ulIndexCacheInfoId;
	ULONG 		ulSetCacheSize;		// 总大小(MB)
	ULONG 		ulSetBlockSize;		// 单块大小(KB)
	WCHAR 		wSetSSDDriverLetter;// 盘符
	ULONG 		ulSetSSDCacheSize;	// 总大小(MB)

	ULONG		ulCacheSize;		// 总大小(扇区数)
	ULONG		ulBlockSize;		// 单块大小(扇区数)
	ULONG		ulUserCount;		// 块使用个数
	ULONGLONG	ullAllReadSize;		// 总读取
	ULONGLONG	ullCacheReadSize;	// 缓存读取
	ULONGLONG	ullAllWriteSize;	// 总写入
	ULONGLONG	ullCacheWriteSize;	// 缓存写入
	ULONG		ulDiskNumber;
} CACHESTATISTICS, *PCACHESTATISTICS;

typedef struct tagSetAddCacheParam
{
	WCHAR wDriverLetter;			// 盘符
	ULONGLONG ullStartSector;		// 开始扇区
	ULONG ulNumberSector;			// 扇区个数
} SETADDCACHEPARAM, *PSETADDCACHEPARAM;

typedef struct tagBlockNumberInfo
{
	ULONG ulCurPrvNumber;
	ULONG ulCurNexNumber;
	ULONG ulNumber;
	ULONGLONG ullDLinkSize;
} BLOCKNUMBERINFO, *PBLOCKNUMBERINFO;

typedef struct tagBlockNumberInfox
{
	ULONG ulCurPrvNumber0;
	ULONG ulCurNexNumber0;
	ULONG ulNumber0;
	ULONG ulNumberUsed0;
	ULONG ulCurPrvNumber1;
	ULONG ulCurNexNumber1;
	ULONG ulNumber1;
	ULONG ulNumberUsed1;
	ULONG ulCurPrvNumber2;
	ULONG ulCurNexNumber2;
	ULONG ulNumber2;
	ULONG ulNumberUsed2;
	ULONG ulCurPrvNumber3;
	ULONG ulCurNexNumber3;
	ULONG ulNumber3;
	ULONG ulNumberUsed3;
	ULONGLONG ullDLinkSize;
	ULONGLONG ullAllUseCount;		// 总访问计数
	ULONGLONG ullAllNoUseCount;		// 总删除计数
	ULONGLONG ullAllRemoveCount;	// 总淘汰计数
	ULONGLONG ullAllMoveCount;		// 总移动计数
	ULONGLONG ullAllLoopCount;		// 总循环计数
	ULONG ulAccessCountB0;		// 块访问次数
	ULONG ulAccessTimeB0;		// 块访问时间
	ULONG ulAccessCountE0;		// 块访问次数
	ULONG ulAccessTimeE0;		// 块访问时间
	ULONG ulAccessCountB1;		// 块访问次数
	ULONG ulAccessTimeB1;		// 块访问时间
	ULONG ulAccessCountE1;		// 块访问次数
	ULONG ulAccessTimeE1;		// 块访问时间
	ULONG ulMaxAccessCount;		// 替换块访问次数
	ULONG ulCount[15];
} BLOCKNUMBERINFOX, *PBLOCKNUMBERINFOX;

typedef struct tagBlockListItem
{
	ULONGLONG ullStartSector;
	ULONG     ulAccessCount;
} BLOCKLISTITEM, *PBLOCKLISTITEM;

#pragma pack(pop)

/// 开启NT驱动
DWORD StartNTDriver(LPCTSTR lpDriverName = SERVICE_NAME);

/// 关闭NT驱动
DWORD StopNTDriver(LPCTSTR lpDriverName = SERVICE_NAME);

/// 安装NT驱动
DWORD InstallNTDriver(LPCTSTR lpDriverName = SERVICE_NAME);

/// 卸载NT驱动
DWORD UnInstallNTDriver(LPCTSTR lpDriverName = SERVICE_NAME);

/// 开启缓存
DWORD StartCache(PSTARTCACHEPARAM pStartCacheParam, DWORD dwSize);

/// 停止缓存
DWORD StopCache(ULONG ulIndexCacheInfoId);

/// 更新缓存
DWORD UpdateCache(DWORD dwIndexCacheInfoId, PDRIVERLETTERINFO pDriverLetterInfo, DWORD dwCount);

/// 获取缓存信息
DWORD GetCacheInfo(void *pBuffer, DWORD dwSize);

/// 获取缓存信息
DWORD CheckCacheInfo(PBOOLEAN pbEnable, WCHAR wDriverLetter = 0);

/// 设置添加CACHE
DWORD IoSetAddCache(WCHAR wDriverLetter, ULONGLONG ullStartSector, ULONG ulNumberSector, PHANDLE m_phCacheHandle = NULL);

/// 获取缓存块数量信息
DWORD GetBlockNumberInfo(ULONG ulIndexCacheInfoId, PBLOCKNUMBERINFO pBlockNumberInfo);

DWORD GetBlockNumberInfoX(ULONG ulIndexCacheInfoId, PBLOCKNUMBERINFOX pBlockNumberInfo);
DWORD GetBlockListInfo(ULONG ulIndexCacheInfoId, PBLOCKLISTITEM pBlockInfo, PULONG pCount);