#pragma once

#include "win32_disk_device.h"

START_NAMESPACE_NTFS

#pragma pack(push, 1)
class IndexBPlusTree;
struct tagNtfsDiskIndexParam : tagNtfsDiskParam
{
	TCHAR szDiskFile[MAX_PATH];
	unsigned __int64 DiskSize;
	IndexBPlusTree *pIndexRead;
	IndexBPlusTree *pIndexWrite;
	bool FileFlagNoBuffering;
	bool isFactFile;
	tagNtfsDiskIndexParam()
		: pIndexRead(NULL)
		, pIndexWrite(NULL)
		, DiskSize(0)
		, FileFlagNoBuffering(false)
		, isFactFile(false)
	{
		::memset(szDiskFile, 0, sizeof(szDiskFile));
	}
};
#pragma pack(pop)


class NtfsIndexFileDevice : public NtfsDiskDevice
{
public:

	NtfsIndexFileDevice();

	~NtfsIndexFileDevice();

public:

	/// 初始化
	virtual DWORD Init(void *pParam = NULL, unsigned long flags = NTFS_IMP_NONE, const char *name  = NULL);

	/// 释放
	virtual void Release();

public:

	/// 检查入参是否有效
	virtual bool CheckParamInfo(void *pParam);

	/// 读数据
	virtual unsigned long on_device_readdataformdisk(void *pParam, unsigned __int64 ullOffSet, void *pBuffer, unsigned long dwSize);

	/// 写数据
	virtual unsigned long on_device_writedatatodisk(void *pParam, unsigned __int64  ullOffSet, const void *pBuffer, unsigned long dwSize);

public:

	/// 初始化
	virtual signed long on_device_init(void *pParam);

	/// 释放
	virtual signed long on_device_release(void *pParam);

	/// 刷新磁盘
	virtual signed long on_device_sync(void *pParam);
};

END_NAMESPACE_NTFS
