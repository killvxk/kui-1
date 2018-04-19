#pragma once

#include "ntfsimp.h"

START_NAMESPACE_NTFS

#pragma pack(push, 1)
struct tagNtfsDiskParam
{
	unsigned long SectorSize;
	unsigned char szDiskBootInfo[SECTOR_SIZE];
	unsigned __int64 TotalReadSize;
	unsigned __int64 TotalWriteSize;
	unsigned __int64 ullOffSet;
	int iVolumeIndex;
	tagNtfsDiskParam()
		: SectorSize(SECTOR_SIZE)
		, TotalReadSize(0)
		, TotalWriteSize(0)
		, ullOffSet(0)
		, iVolumeIndex(-1)
	{
		::memset(szDiskBootInfo, 0, sizeof(szDiskBootInfo));
	}
};
#pragma pack(pop)


class NtfsDiskDevice : public NtfsImp
{
public:

	NtfsDiskDevice();

	virtual ~NtfsDiskDevice();

public:

	/// 初始化
	virtual DWORD Init(void *pParam = NULL, unsigned long flags = NTFS_IMP_NONE, const char *name  = NULL);

	/// 释放
	virtual void Release();

public:

	/// 检查入参是否有效
	virtual bool CheckParamInfo(void *pParam) = 0;

	/// 读数据
	virtual unsigned long on_device_readdataformdisk(void *pParam, unsigned __int64 ullOffSet, void *pBuffer, unsigned long dwSize) = 0;

	/// 写数据
	virtual unsigned long on_device_writedatatodisk(void *pParam, unsigned __int64  ullOffSet, const void *pBuffer, unsigned long dwSize) = 0;

public:

	/// 初始化
	virtual signed long on_device_init(void *pParam);

	/// 释放
	virtual signed long on_device_release(void *pParam);

	/// 设置单个扇区大小
	virtual signed long on_device_setsectorsize(void *pParam, unsigned short bytespersector);

	/// 读数据
	virtual signed long on_device_readdata(void *pParam, unsigned __int64 ullOffSet, void *pBuffer, unsigned long dwSize);

	/// 写数据
	virtual signed long on_device_writedata(void *pParam, unsigned __int64 ullOffSet, const void *pBuffer, unsigned long dwSize);

	/// 读扇区
	virtual signed long on_device_readsector(void *pParam, unsigned __int64 ullStartSector,  unsigned long dwNumber, void *pBuffer);

	/// 写扇区
	virtual signed long on_device_writesector(void *pParam, unsigned __int64  ullStartSector, unsigned long dwNumber, const void *pBuffer);

	/// 刷新磁盘
	virtual signed long on_device_sync(void *pParam);

	/// 相对地址与绝对地址转换
	virtual signed long on_device_daddresstofaddress(void *pParam, unsigned __int64  ullOffSet,
		unsigned __int64*  pullOffSet);

public:

	/// 判断是否EFI启动(pMbrBuffer:第一个扇区数据，pGptBuffer:第二个扇区数据)
	static signed long checkefiboot(const void *pMbrBuffer, const void *pGptBuffer);
};

END_NAMESPACE_NTFS
