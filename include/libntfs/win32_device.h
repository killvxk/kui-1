#pragma once

#include "ntfsimp.h"

START_NAMESPACE_NTFS

struct tagNtfsDriverParam
{
	TCHAR szDriver;
	HANDLE hDriver;				// 卷句柄
	HANDLE hReadWriteEvent;		// 读写句柄
	unsigned char  *pUnalignedBuf;
	NtfsLock lockReadWrite;
	tagNtfsDriverParam()
		: szDriver(L'\0')
		, hDriver(INVALID_HANDLE_VALUE)
		, hReadWriteEvent(NULL)
		, pUnalignedBuf(NULL)
	{
	}
};

class NtfsDriver : public NtfsImp
{
public:

	NtfsDriver();

	virtual ~NtfsDriver();

public:

	/// 初始化
	virtual DWORD Init(void *pParam = NULL, unsigned long flags = NTFS_IMP_RDONLY, const char *name  = NULL);

	/// 释放
	virtual void Release();

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

private:

	/// 读数据
	signed long on_device_readdataex(void *pParam, unsigned __int64 ullOffSet, void *pBuffer, unsigned long dwSize);

	/// 写数据
	signed long on_device_writedataex(void *pParam, unsigned __int64  ullOffSet, const void *pBuffer, unsigned long dwSize);

};

END_NAMESPACE_NTFS
