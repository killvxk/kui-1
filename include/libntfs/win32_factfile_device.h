#pragma once

#include "win32_disk_device.h"

START_NAMESPACE_NTFS

#pragma pack(push, 1)
struct tagNtfsFactFileParam : tagNtfsDiskParam
{
	TCHAR szDiskFile[MAX_PATH];
	HANDLE hFile;				// 句柄
	HANDLE hReadWriteEvent;		// 读写句柄
	NtfsLock lockReadWrite;
	unsigned char* pDataAlignedBuf;
	unsigned long DataAlignedSize;
	bool FileFlagNoBuffering;
	tagNtfsFactFileParam()
		: hFile(INVALID_HANDLE_VALUE)
		, hReadWriteEvent(NULL)
		, pDataAlignedBuf(NULL)
		, DataAlignedSize(SECTOR_SIZE) // 512B
		, FileFlagNoBuffering(false)
	{
		::memset(szDiskFile, 0, sizeof(szDiskFile));
	}
};

#pragma pack(pop)


class NtfsFactFileDevice : public NtfsDiskDevice
{
public:

	NtfsFactFileDevice();

	virtual ~NtfsFactFileDevice();

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

private:

	/// 读数据
	signed long on_device_readdataex(void *pParam, unsigned __int64 ullOffSet, void *pBuffer, unsigned long dwSize);

	/// 写数据
	signed long on_device_writedataex(void *pParam, unsigned __int64  ullOffSet, const void *pBuffer, unsigned long dwSize);

};

END_NAMESPACE_NTFS
