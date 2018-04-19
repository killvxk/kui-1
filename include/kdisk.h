#ifndef __KDisk_inc_h__
#define __KDisk_inc_h__

#pragma once

#define KDISK_NTDEVICE_NAME		L"\\Device\\KDiskVol%c"
#define KDISK_DOSDEVICE_NAME	L"\\DosDevices\\KDiskVol%c"

#define KDISK_WIN32_NAME		TEXT("\\\\.\\KDiskVol%c")
#define KDISK_DRIVER_NAME_X86	TEXT("kdisk.sys")
#define KDISK_DRIVER_NAME_X64	TEXT("kdisk64.sys")
#define KDISK_SERVICE_NAME		TEXT("kdisk")

#define IOCTL_KDISK_MOUNT		CTL_CODE(0x8000, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS|FILE_WRITE_ACCESS)
#define IOCTL_KDISK_UNMOUNT		CTL_CODE(0x8000, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS|FILE_WRITE_ACCESS)

typedef struct MOUNTINFO
{
	ULONG   DiskType;
	ULONG	Ip;
	USHORT	Port;
	WCHAR	UserId[64];
}MOUNTINFO, *PMOUNTINFO;

#define CONNECT_TYPE_UNKOWN				0		// 未知连接
#define CONNECT_TYPE_FILEDISK			1		// 文件磁盘连接 (服务器PNP使用)
#define CONNECT_TYPE_USERMAIN			2		// 虚拟盘主连接 (虚拟磁盘使用)
#define CONNECT_TYPE_USERREAD			3		// 虚拟盘预读连接 (预读使用)
#define CONNECT_TYPE_ADMIN				4		// 管理员：不删除临时文件(创建还原点)
#define CONNECT_TYPE_SUPER_ADMIN		5		// 超级管理员：直接操作镜像系统(服务器挂载及上传工具使用)

#define FRAME_START_FLAG			'.mgs'
#define FRAME_CMD_QUERY				0x01
#define FRAME_CMD_READ				0x02
#define FRAME_CMD_WRITE				0x03
#define FRAME_CMD_INFO				0x04

#pragma warning(disable:4200)

#pragma pack(push, 1)

typedef struct VDiskRequest
{
	ULONG			flag;		// FRAME_START_FLAG
	ULONG			length;		// total length.
	ULONG			command;	// 命令号
	ULONG			status;		// 状态

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
		}Read;
		struct
		{
			LARGE_INTEGER	start;
			ULONG			number;
		}Write;
		struct  
		{
			UCHAR			type;	// 连接类型
		}ConnectInfo;
	};
	CHAR			data[0];		// 数据
}VDISKREQUEST,*LPVDISKREQUEST;

typedef struct VDiskAck
{
	ULONG			flag;			// FRAME_START_FLAG
	ULONG			length;			// total length.
	ULONG			command;		// 命令号
	ULONG			status;			// 状态
	CHAR			data[0];		// 数据
}VDISKACK, *LPVDISKACK;

typedef struct WriteBackSettings
{
	ULONG			mem_size;		//  扇区数
	ULONG			file_size;		//  扇区数
	ULONG			file_drive;
	ULONG			reserved[4];
}WRITEBACK_SETTINGS, *PWRITEBACK_SETTINGS;

typedef struct PreReadSettings
{
	UCHAR	bIsPreRead;
	UCHAR	bBlockSize;				// K
	UCHAR	bPreReadThread;			// 预读线程
	USHORT	wReadOneCacheSize;		// K
	USHORT	wAllReadCacheSize;		// K
}PREREADSETTINGS, *PPREREADSETTINGS;

typedef struct tPtItem
{
	UCHAR	bState;
	UCHAR	bStartHead;
	UCHAR	bStartSec;
	UCHAR	bStartCyl;
	UCHAR	bType;
	UCHAR	bEndHead;
	UCHAR	bEndSec;
	UCHAR	bEndCyl;
	ULONG	dwStartSec;
	ULONG	dwTotalSecs;
}PTITEM, *LPPTITEM;

#pragma pack(pop)

#endif