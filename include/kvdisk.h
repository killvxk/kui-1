#pragma once

#include "kcore.h"

START_NAMESPACE_KCORE

#define MAX_RESTORE_NUM					32		// 最大还原点个数

// 与控制台交互命令
#define COM_SM_HEART					0x0001		// 控制台交互的心跳包(无)
#define COM_SM_ADDIMAGE					0x0002		// 添加镜像(输入:name(string) + size(DWORD)<单位:G> + cachesize(ULONGLONG)<单位:B>)
#define COM_SM_ADDRESTPOINT				0x0003		// 添加还原点(输入:add(bool) + ip(DWORD) + id(DWORD) + name(string))
#define COM_SM_DELIMAGE					0x0004		// 删除镜像(输入:cout(DWORD) + delete(bool) + array of id(DWORD))
#define COM_SM_DELRESTPOINT				0x0005		// 删除还原点(输入:id(DWORD) + rstid(DWORD) + deletefile(bool))
#define COM_SM_MODIFYIMAGE				0x0006		// 修改镜像信息(输入:id(DWORD) + cachesize(ULONGLONG)<单位:B>)
#define COM_SM_SETAPPLYPOINT			0x0007		// 设置还原点当前应用(输入:id(DWORD) + rstid(DWORD))
#define COM_SM_MERGERESTPOINT			0x0008		// 合并还原点(输入:id(DWORD) + rstid(DWORD) + type((DWORD)0:合并 1:状态 2:取消合并)
#define COM_SM_CHECKPASWORD				0x0009		// 检查上传工具密码(输入:password(string)); 输出:count(DWORD) + array of (id(DWORD) + name(string) + port(WORD)
#define COM_SM_GETCLIENTEX				0x000a		// 获取客户机统计信息(输出: count(DWORD) + array of stat(tagStatistics)
#define COM_SM_GETGAMEDISKEX			0x000b		// 获取游戏盘统计信息(输入:ip(DWORD) + driver(TCHAR) + detail(bool)) (输出: maxsendspeed(DWORD) + sendspeed(DWORD) + totalsendbytes(ULONGLONG) + count(DWORD) + array of stat(tagStatistics)
#define COM_SM_GETCACHEINFOEX			0x000c		// 获取缓存统计信息(输入:count(DWORD) + array of driver(TCHAR)) (输出:count(DWORD) + array of (driver(TCHAR) + start(bool) + hit(double)))
#define COM_SM_GETHOSTMEMORY			0x000d		// 获取本机内存信息(输出:total(DWORD) + free(DWOR)
#define COM_SM_NOTIFYCACHE				0x000e		// 刷新缓存(无)
#define COM_SM_SETDHCPANDTFTP			0x000f		// 设置是否开启DHCP、TFTP服务(输入:enable(bool))
#define COM_SM_MIRRORREADFILE			0x0010		// 读取镜像文件数据(输入:id(DWORD) + restid(DWORD) + offset(LARGE_INTEGER) + size(DWORD) + isdatafile(bool) 输出: size(DWORD) + data(char*)
#define COM_SM_GETALLMIRRORINFOEX		0x0011		// 获取镜像信息(输出: count(DWORD) + array of info(MIRRORINFOEX)
#define COM_SM_NOTIFYMIRRORSYNC			0x0012		// 通知镜像同步
#define COM_SM_GETSUPERADMINIP			0x0013		// 获取超级管理员的IP(输入: id(DWORD) 输入: ip(DWORD))
#define COM_SM_GETALLMIRRORINFO			0x0014		// 获取镜像信息(输出: count(DWORD) + array of info(id(DWORD) + restnum(DWORD) + lastrestid(DWORD) + userestid(DWORD))
#define COM_SM_SETMIRRORDELETE			0x0015		// 设置镜像卸载时文件删除状态(输入:cout(DWORD) + delete(bool) + array of id(DWORD))
#define COM_SM_CLIENTOPTIONADMIN		0x0016		// 客户机设置管理员(输入:mirrorid(DWORD) + machinename(string) + startorstop(bool) + saveorcancel(bool) + rspname(string))
#define COM_SM_CLIENTGETIMAGEINFO		0x0017		// 客户机获取镜像信息(输入:password(string)) + machinename(string); 输出: supermirrorid(DWORD) + issave(bool) + count(DWORD) + array of (id(DWORD) + name(string))
#define COM_SM_GETALLMIRRORINFOEXEX		0x0018		// 获取镜像信息(输出: flag(DWORD) + count(DWORD) + array of info(MIRRORINFOEX)
#define COM_SM_GETLOCALPNPINFO			0x0019		// 获取本地PNP信息(输入: VenorId(WORD) + DeviceId(WORD) + SubSys(DWORD) + Rev(BYTE) + Os(string))
													// 输出: count(DWORD) + array of info(name(string) + size(LARGE_INTEGER) + time(FILETIME))
#define COM_SM_LOCALPNPREADFILE			0x001a		// 读取本地文件数据(输入: VenorId(WORD) + DeviceId(WORD) + SubSys(DWORD) + Rev(BYTE) + Os(string) + name(string) + offset(LARGE_INTEGER) + size(DWORD))
													// 输出: size(DWORD) + data(char*)
#define COM_SM_NOTIFYLOCALPNPSYNC		0x001b		// 通知本地PNP同步
#define COM_SM_INFTOLOCALPARAM			0x001c		// 设置INF到本地PNP参数(输入: VenorId(WORD) + DeviceId(WORD) + SubSys(DWORD) + Rev(BYTE) + Os(string) + DbOpt(DWORD))
#define COM_SM_REMOVELOCALPNP			0x001d		// 删除本地PNP (输入: count(DWORD) + array of info(VenorId(WORD) + DeviceId(WORD) + SubSys(DWORD) + Rev(BYTE) + Os(string))
#define COM_SM_MIRROR_READ_DATA			0x001e		// 从镜像读取数据
#define COM_SM_SSD_PNP					0x001f		// 制作SSDPNP
#define COM_SM_PNP_FILE_LIST			0x0020		// PNP文件列表
#define COM_SM_GETGAMEDISKNEW			0x0021		// 获取新游戏盘统计信息(输入:ip(DWORD) + driver(TCHAR) + detail(bool)) (输出: maxsendspeed(DWORD) + sendspeed(DWORD) + totalsendbytes(ULONGLONG) + maxrecvspeed(DWORD) + recvspeed(DWORD) + totalrecvbytes(ULONGLONG) + count(DWORD) + array of stat(tagStatistics)

//显卡pnp
#define COM_SM_USERDISKREADFILE			0x0022		// 读取显卡镜像文件数据(输入:id(DWORD) + restid(DWORD) + offset(LARGE_INTEGER) + size(DWORD) + isdatafile(bool) 输出: size(DWORD) + data(char*)
#define COM_SM_GETALLUSERDISKINFOEX		0x0023		// 获取显卡镜像信息(输出: USERDISKINFOEX

#define COM_SM_INFTOLOCALDPARAM			0x0024		// 设置显卡INF到本地PNP参数(输入: VenorId(WORD) + DeviceId(WORD) + SubSys(DWORD) + Rev(BYTE) + Os(string) + DbOpt(DWORD))
#define COM_SM_GETLOCALDPNPINFO			0x0025		// 获取本地PNP信息(输入: VenorId(WORD) + DeviceId(WORD) + SubSys(DWORD) + Rev(BYTE) + Os(string))
													// 输出: count(DWORD) + array of info(name(string) + size(LARGE_INTEGER) + time(FILETIME))
#define COM_SM_LOCALDPNPREADFILE		0x0026		// 读取本地文件数据(输入: VenorId(WORD) + DeviceId(WORD) + SubSys(DWORD) + Rev(BYTE) + Os(string) + name(string) + offset(LARGE_INTEGER) + size(DWORD))
													// 输出: size(DWORD) + data(char*)

#define COM_SM_NOTIFYMAKEDPNP			0x0027      //通知制作dpnp
#define COM_SM_REMOVELOCALDPNP			0x0028		// 删除本地PNP (输入: count(DWORD) + array of info(VenorId(WORD) + DeviceId(WORD) + SubSys(DWORD) + Rev(BYTE) + Os(string))
#define COM_SM_NOTIFYREMAKEUSERDISK		0x0029      //通知重新制作UserDisk(输入:type((DWORD)0:制作 1:状态 2:取消制作)
#define COM_SM_NOTIFYUSERDISKSYNC		0x002A		// 通知显卡镜像同步

#define COM_SM_NOTIFYDBOPTION			0x002B      //通知获取数据库系统选项
#define COM_SM_REPORTSVRIPINFO			0x002C		//上报服务器ip信息（输入：tstring + count(DWORD) + array of info(SVRIPINFO)
													//输出（std::map<tstring, SVRIPINFOVEC>)
// 返回状态
#define STATUS_NOTFINDIMAGEDRIVER		0x0001		// 没有发现镜像盘
#define STATUS_IMAGENAMEEXIST			0x0002		// 镜像已经存在
#define STATUS_IMAGESTARTFAIL			0x0003		// 镜像启动失败
#define STATUS_UNKOWNERROR				0x0004		// 未知错误
#define STATUS_INSERTDBERROR			0x0005		// 插入数据库失败
#define STATUS_IMAGENOTEXIST			0x0006		// 镜像不存在
#define STATUS_PASSWORDERROR			0x0007		// 密码错误
#define STATUS_PERMISSIONDENIED			0x0008		// 权限不够，操作禁止
#define STATUS_GETDBERROR				0x0009		// 读取数据库失败
#define STATUS_RSPOINTNOTEXIST			0x000a		// 还原点不存在
#define STATUS_RSPOINTSTARTFAIL			0x000b		// 还原点启动失败
#define STATUS_TOOMORERSPOINT			0x000c		// 太多的还原点
#define STATUS_CACHESTARTFAIL			0x000d		// 缓存启动失败
#define STATUS_HAVEMERGERSPOINT			0x000e		// 已有合并操作正在运行
#define STATUS_DELDBERROR				0x000f		// 删除数据库记录失败
#define STATUS_PARAMINVALID				0x0010		// 参数无效
#define STATUS_NOTENOUGHFREESPACE		0x0011		// 没有足够的剩余空间
#define STATUS_SUPERADMINEXIST			0x0012		// 存在超级管理员
#define STATUS_ADMINEXIST				0x0013		// 存在管理员
#define STATUS_HAVEADMIN				0x0014		// 已是管理员
#define STATUS_NOTONLINE				0x0015		// 客户机不在线
#define STATUS_OPTFORBID				0x0016		// 禁止客户机上传或开超级
#define STATUS_LOCALPNPNOFILE			0x0017		// 本地PNP文件不存在


// 镜像同步状态
#define MIRRORUPDATE_FINISH				0x0001
#define MIRRORUPDATE_ERROR				0x0002
#define MIRRORUPDATE_WAITING			0x0003		// 正在排队
#define MIRRORUPDATE_CHECKSPACE			0x0004		// 检查磁盘空间
#define MIRRORUPDATE_CONNECT			0x0005		// 与主服务器通讯
#define MIRRORUPDATE_INDEXUPDATE		0x0006		// 索引文件同步
#define MIRRORUPDATE_DATAUPDATE			0x0007		// 数据文件同步

// 本地PNP同步状态
#define LOCALPNPUPDATE_FINISH			0x0001
#define LOCALPNPUPDATE_ERROR			0x0002
#define LOCALPNPUPDATE_WAITING			0x0003		// 正在排队
#define LOCALPNPUPDATE_CONNECT			0x0004		// 与主服务器通讯
#define LOCALPNPUPDATE_DATAUPDATE		0x0005		// 数据文件同步


// 本地显卡同步状态
#define USERDISKUPDATE_FINISH				0x0001
#define USERDISKUPDATE_ERROR				0x0002
#define USERDISKUPDATE_WAITING				0x0003		// 正在排队
#define USERDISKUPDATE_CHECKSPACE			0x0004		// 检查磁盘空间
#define USERDISKUPDATE_CONNECT				0x0005		// 与主服务器通讯
#define USERDISKUPDATE_INDEXUPDATE			0x0006		// 索引文件同步
#define USERDISKUPDATE_DATAUPDATE			0x0007		// 数据文件同步

// 接收块大小
#define BLOCK_SIZE						0x100000L
// 一扇区占大小
#define SECTOR_SIZE						512


// 连接类型
#define CONNECT_TYPE_UNKOWN				0		// 未知连接
#define CONNECT_TYPE_FILEDISK			1		// 文件磁盘连接 (服务器PNP使用)
#define CONNECT_TYPE_USERMAIN			2		// 虚拟盘主连接 (虚拟磁盘使用)
#define CONNECT_TYPE_USERREAD			3		// 虚拟盘预读连接 (预读使用)
#define CONNECT_TYPE_ADMIN				4		// 管理员：不删除临时文件(创建还原点)
#define CONNECT_TYPE_SUPER_ADMIN		5		// 超级管理员：直接操作镜像系统(服务器挂载及上传工具使用)

/// 消息号
#define FRAME_START_FLAG				'.mgs'
#define FRAME_CMD_QUERY					0x01
#define FRAME_CMD_READ					0x02
#define FRAME_CMD_WRITE					0x03
#define FRAME_CMD_INFO					0x04
#define FRAME_CMD_STOP					0x05
#define FRAME_CMD_HEART					0x06
#define FRAME_CMD_WRITEBACK_ACTION		0x07
#define FRAME_CMD_GAMEDISKINFO			0x08
#define FRAME_CMD_READ_EX				0x09
#define FRAME_CMD_WRITE_EX				0x0a
#define FRAME_CMD_READ_NEW				0x0b
#define FRAME_CMD_WRITE_NEW				0x0c
#define FRAME_CMD_QUERY_NEW				0x0d
#define FRAME_CMD_INFO_NEW				0x0e
#define FRAME_CMD_HEART_NEW				0x0f
#define FRAME_CMD_GAMEDISKINFO_NEW		0x10
#define FRAME_CMD_WRITEBACK_ACTION_NEW	0x11

#pragma warning(disable:4200)

#pragma pack(push, 1)

/// 磁盘数据头部
typedef struct tagVDiskHeader
{
	ULONG			flag;		// FRAME_START_FLAG
	ULONG			length;		// total length
	ULONG			command;	// 命令号
	ULONG			status;		// 状态
} VDISKHEADER, *LPVDISKHEADER;

/// 磁盘接收数据
typedef struct VDiskRequest : public tagVDiskHeader
{
	union
	{
		struct
		{
			ULONG			resv1[8];
		}Param;
		struct
		{
			LARGE_INTEGER	start;
			ULONG			number;
			LONGLONG		pkg_id;
		}Read;
		struct
		{
			LARGE_INTEGER	start;
			ULONG			number;
			LONGLONG		pkg_id;
		}Write;
		struct  
		{
			UCHAR			type;		// 连接类型
			DWORD			rspid;		// 还原点ID
			DWORD			remoteip;	// 客户机IP
			UCHAR			isreconnect;// 是否重连
		}ConnectInfo;
	};

	CHAR			data[0];		// 数据

} VDISKREQUEST,*LPVDISKREQUEST;

/// 磁盘接收数据
typedef struct VDiskRequestNew : public tagVDiskHeader
{
	union
	{
		struct
		{
			ULONG			resv1[8];
		}Param;
		struct
		{
			LONGLONG		pkg_id;
			LARGE_INTEGER	start;
			ULONG			number;
		}Read;
		struct
		{
			LONGLONG		pkg_id;
			LARGE_INTEGER	start;
			ULONG			number;
		}Write;
		struct
		{
			LONGLONG		pkg_id;
			UCHAR			type;		// 连接类型
			DWORD			rspid;		// 还原点ID
			DWORD			remoteip;	// 客户机IP
			UCHAR			isreconnect;// 是否重连
		}ConnectInfo;
		struct
		{
			LONGLONG		pkg_id;
		}Heart, Query, GameDiskInfo, WritebackAction;
	};

	CHAR			data[0];		// 数据

} VDISKREQUESTNEW, *LPVDISKREQUESTNEW;

/// 数据应答
typedef struct VDiskAck : public tagVDiskHeader
{
	CHAR			data[0];		// 数据
} VDISKACK, *LPVDISKACK;

typedef struct VDiskAckEx : public tagVDiskHeader
{
	LARGE_INTEGER	sector;		//in sectors
	ULONG			number;		//in sectors
	CHAR			data[0];
} VDISKACKEX, *LPVDISKACKEX;

typedef struct VDiskAckNew : public tagVDiskHeader
{
	union
	{
		struct
		{
			ULONG			resv1[8];
		}Param;
		struct
		{
			LONGLONG		pkg_id;
			LARGE_INTEGER	start;
			ULONG			number;
		}Read;
		struct
		{
			LONGLONG		pkg_id;
			LARGE_INTEGER	start;
			ULONG			number;
		}Write;
		struct
		{
			LONGLONG		pkg_id;
		}ConnectInfo, Heart, Query, GameDiskInfo, WritebackAction;
	};

	CHAR			data[0];		// 数据
} VDISKACKNEW, *LPVDISKACKNEW;


typedef struct WriteBackSettings
{
	ULONG			mem_size;		//  扇区数
	ULONG			file_size;		//  扇区数
	ULONG			file_drive;		//	文件盘符
	ULONG			use_unrecognized;// 使用未识别内存
	ULONG			writeback_limit; //回写限制 in Mbytes
	ULONG			reserved[2];
}WRITEBACK_SETTINGS, *PWRITEBACK_SETTINGS;

typedef struct PreReadSettings
{
	BYTE	bIsPreRead;
	BYTE	bBlockSize;				// K
	BYTE	bPreReadThread;			// 预读线程
	WORD	wReadOneCacheSize;		// K
	WORD	wAllReadCacheSize;		// K
}PREREADSETTINGS, *PPREREADSETTINGS;

#pragma pack(pop)

#define SENDREVBUFFER_SIZE	BLOCK_SIZE + sizeof(VDiskRequest)

#define IMAGE_PATH_NAME		TEXT("kimage")
#define WRITEBACK_PATH_NAME	TEXT("kwrite")
#define USERDISK_NAME		TEXT("userdisk")

END_NAMESPACE_KCORE