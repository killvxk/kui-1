#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#include "ntfspublic.h"

struct _ntfs_volume;
struct ntfs_device_operations;
struct ntfs_device;

START_NAMESPACE_NTFS

enum 
{
	NTFS_IMP_NONE                   = 0x00000000,
	NTFS_IMP_RDONLY                 = 0x00000001,
};

// 单位:(单位:字节)
typedef struct tagFileClusters
{
	unsigned __int64 ul64Vcn;			// 文件相对地址
	unsigned __int64 ui64Lcn;			// 实际地址
	unsigned __int64 ui64Length;		// 总大小
} FILECLUSTERS, *PFILECLUSTERS;

class NtfsImp
{
public:

	NtfsImp();

	virtual ~NtfsImp();

public:

	/// 初始化
	virtual DWORD Init(void *pParam = NULL, unsigned long flags = NTFS_IMP_NONE, const char *name  = NULL);

	/// 释放
	virtual void Release();

public:

	/// 初始化
	virtual signed long on_device_init(void *pParam) = 0;

	/// 释放
	virtual signed long on_device_release(void *pParam) = 0;

	/// 设置单个扇区大小
	virtual signed long on_device_setsectorsize(void *pParam, unsigned short bytespersector) = 0;

	/// 读数据
	virtual signed long on_device_readdata(void *pParam, unsigned __int64 ullOffSet, void *pBuffer, unsigned long dwSize) = 0;

	/// 写数据
	virtual signed long on_device_writedata(void *pParam, unsigned __int64 ullOffSet, const void *pBuffer, unsigned long dwSize) = 0;

	/// 读扇区
	virtual signed long on_device_readsector(void *pParam, unsigned __int64 ullStartSector,  unsigned long dwNumber, void *pBuffer) = 0;

	/// 写扇区
	virtual signed long on_device_writesector(void *pParam, unsigned __int64  ullStartSector, unsigned long dwNumber, const void *pBuffer) = 0;

	/// 刷新磁盘
	virtual signed long on_device_sync(void *pParam) = 0;

	/// 相对地址与绝对地址转换
	virtual signed long on_device_daddresstofaddress(void *pParam, unsigned __int64  ullOffSet,
		unsigned __int64*  pullOffSet) = 0;

public:

	/// 创建文件夹(pszDirPath= .\test\path)
	DWORD CreateDirectory(LPCTSTR pszDirPath);

	/// 创建文件(pszFilePath= .\test\path\file)
	DWORD CreateFile(LPCTSTR pszFilePath, HANDLE *phFile = NULL);

	/// 写入文件数据
	DWORD WriteFile(HANDLE hFile, LPCVOID lpBuffer, ULONGLONG ullOffset, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWrite);

	/// 删除文件(pszFilePath= .\test\path\file)
	DWORD DeleteFile(LPCTSTR pszFilePath);

	/// 获取文件大小
	DWORD SetFileSize(HANDLE hFile, ULONGLONG ullSize);

	/// 拷贝磁盘文件到NTFS(lpNtfsFileName= .\test\path\file)
	DWORD CopyFile(LPCTSTR lpExistingFileName, LPCTSTR lpNtfsFileName, HANDLE hEvent = NULL);

	/// 拷贝磁盘文件到NTFS(lpNtfsFileName= .\test\path\file)
	typedef DWORD(*COPYFILECALLBACK)(LPVOID pParam, LPCTSTR lpSrc, LPCTSTR lpDest,
		DWORD dwWriteBytes, LPCTSTR szError);
	DWORD CopyFile(LPCTSTR lpExistingFileName, LPCTSTR lpNtfsFileName,
		COPYFILECALLBACK pCopyFileCallBack = NULL, LPVOID pParam = NULL);

	/// 拷贝NTFS文件到磁盘文件(lpNtfsFileName= .\test\path\file)
	DWORD CopyFileEx(LPCTSTR lpNtfsFileName, LPCTSTR lpFileName, HANDLE hEvent = NULL);

	/// 拷贝磁盘目录到NTFS(lpNtfsDirName= .\test\path\path)
	DWORD CopyDirectory(LPCTSTR lpExistingDirName, LPCTSTR lpNtfsDirName, HANDLE hEvent = NULL);

	/// 拷贝磁盘目录到NTFS(lpNtfsDirName= .\test\path\path)
	DWORD CopyDirectory(LPCTSTR lpExistingDirName, LPCTSTR lpNtfsDirName,
		COPYFILECALLBACK pCopyFileCallBack = NULL, LPVOID pParam = NULL);

	/// 设置文件时间信息
	DWORD SetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime);

	/// 清除DIRTY标识
	DWORD ClearDirtyFlag();

	/// 重置日志文件
	DWORD ResetLogFile(__int64 i64Size, bool bReset = false);

public:

	/// 打开文件(pszFilePath= .\test\path\file, 文件必须存在)
	DWORD OpenFile(LPCTSTR pszFilePath, HANDLE &hFile);

	/// 关闭文件句柄
	void CloseFile(HANDLE hFile);

	/// 读取文件数据
	DWORD ReadFile(HANDLE hFile, LPVOID lpBuffer, ULONGLONG ullOffset, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);

	/// 获取文件大小
	DWORD GetFileSize(HANDLE hFile, ULONGLONG &ullSize);

	/// 获取文件名称
	DWORD GetFileName(HANDLE hFile, LPTSTR szFileName, DWORD dwLen);

	/// 是否文件夹
	BOOL IsDirectory(HANDLE hFile);
	
	/// 获取文件时间信息
	DWORD GetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime);

	/// 获文件簇信息
	DWORD GetFileCluster(HANDLE hFile, std::vector<FILECLUSTERS> &vtFileClusters);

public:

	/// 获取簇个数
	ULONGLONG GetTotalClusters();

public:

	/// 初始化
	static signed long device_init(struct ntfs_device *dev);

	/// 释放
	static signed long device_release(struct ntfs_device *dev);

	/// 设置单个扇区大小
	static signed long device_setsectorsize(struct ntfs_device *dev, unsigned short bytespersector);

	/// 读数据
	static signed long device_readdata(struct ntfs_device *dev, unsigned __int64 ullOffSet, void *pBuffer, unsigned long dwSize);

	/// 写数据
	static signed long device_writedata(struct ntfs_device *dev, unsigned __int64 ullOffSet, const void *pBuffer, unsigned long dwSize);

	/// 读扇区
	static signed long device_readsector(struct ntfs_device *dev, unsigned __int64 ullStartSector,  unsigned long dwNumber, void *pBuffer);

	/// 写扇区
	static signed long device_writesector(struct ntfs_device *dev, unsigned __int64  ullStartSector, unsigned long dwNumber, const void *pBuffer);

	/// 刷新磁盘
	static signed long device_sync(struct ntfs_device *dev);

private:

	/// 设置设备操作回调
	unsigned long ntfs_set_device_callback(void *init, void *release, void *setsectorsize, void *readdata, 
		void *writedata, void *readsector, void *writesector, void *sync);

private:

	struct ntfs_device_operations *m_pdevice;

	struct _ntfs_volume *m_pVolume;

	void *m_pParam;

};

END_NAMESPACE_NTFS