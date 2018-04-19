#pragma once

#ifdef WIN32

#include <memory>
#include <system_error>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <vector>
#include <functional>
#include <mutex>

#ifndef START_LIBNETIO_NAME_SPACE
#define START_LIBNETIO_NAME_SPACE	namespace libnetio {
#endif
#ifndef END_LIBNETIO_NAME_SPACE
#define END_LIBNETIO_NAME_SPACE		}
#endif

START_LIBNETIO_NAME_SPACE

struct TcpSession;
class IoServicePool;

typedef struct tagTcpSessionFileParam
{
	std::shared_ptr<TcpSession> pSessionPtr;		// TCP客户机
	std::size_t iTotalTrascationLen;				// 总大小
	std::size_t iTrascationLen;						// 拆分大小
	std::mutex lockTrascationLen;					// 拆分大小锁
} TCPSESSIONFILEPARAM, *PTCPSESSIONFILEPARAM;

class IoFile
{
public:

	struct tagIoFileImp;
	using IoFileImpPtr = std::unique_ptr<tagIoFileImp>;

public:

	IoFile();

	virtual ~IoFile();

public:

	/// 开启服务器
	/// (dwDesiredAccess:GENERIC_ALL etc.)
	/// (dwDesiredAccess:OPEN_ALWAYS etc.)
	/// (dwFlagsAndAttributes:FILE_ATTRIBUTE_NORMAL etc.)
	virtual int32_t Start(std::wstring &strFilePath, DWORD dwDesiredAccess,
		DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, std::size_t iThreadCount = 0);

	/// 开启服务器
	virtual int32_t Start(std::wstring &strFilePath,
		std::shared_ptr<IoServicePool> &pIoServicePoolPtr, DWORD dwDesiredAccess,
		DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes);

	/// 开启服务器(hFile must be set FILE_FLAG_OVERLAPPED)
	virtual int32_t Start(HANDLE hFile, std::size_t iThreadCount = 0);

	/// 开启服务器(hFile must be set FILE_FLAG_OVERLAPPED)
	virtual int32_t Start(HANDLE hFile, std::shared_ptr<IoServicePool> &pIoServicePoolPtr);

	/// 停止服务器
	virtual void Stop();

	/// 输出日志
	virtual void WriteLogInfo(const std::error_code &err);

public:

	/// 读取数据后响应(offset:文件偏移;bptoffset:pBufferPtr偏移)
	virtual void OnRead(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t bptoffset, uint64_t offset, std::size_t needsize,
		const std::error_code &err, std::size_t size) = 0;

	/// 写入数据后响应(offset:文件偏移;bptoffset:pBufferPtr偏移)
	virtual void OnWrite(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t bptoffset, uint64_t offset, std::size_t needsize,
		const std::error_code &err, std::size_t size) = 0;

public: /// TCP客户机使用

	/// 读取数据后响应(offset:文件偏移;bptoffset:pBufferPtr偏移)
	virtual void OnTcpSessionRead(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t bptoffset, uint64_t offset, std::size_t needsize,
		std::shared_ptr<tagTcpSessionFileParam> &pTcpSessionFileParamPtr,
		const std::error_code &err, std::size_t size) = 0;

	/// 写入数据后响应(offset:文件偏移;bptoffset:pBufferPtr偏移)
	virtual void OnTcpSessionWrite(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t bptoffset, uint64_t offset, std::size_t needsize,
		std::shared_ptr<tagTcpSessionFileParam> &pTcpSessionFileParamPtr,
		const std::error_code &err, std::size_t size) = 0;

public:

	/// 读取文件数据(offset:文件偏移;bptoffset:pBufferPtr偏移)
	int32_t ReadFile(uint64_t offset, std::shared_ptr<std::vector<char>> &pBufferPtr, 
		std::size_t size, std::size_t bptoffset = 0);

	/// 写入文件数据(offset:文件偏移;bptoffset:pBufferPtr偏移)
	int32_t WriteFile(uint64_t offset, std::shared_ptr<std::vector<char>> &pBufferPtr,
		std::size_t size, std::size_t bptoffset = 0);

public: /// TCP客户机使用

	/// 读取文件数据(offset:文件偏移;bptoffset:pBufferPtr偏移)
	int32_t TcpSessionReadFile(uint64_t offset, 
		std::shared_ptr<std::vector<char>> &pBufferPtr,	std::size_t size, 
		std::shared_ptr<tagTcpSessionFileParam> &pTcpSessionFileParamPtr, 
		std::size_t bptoffset = 0);

	/// 写入文件数据(offset:文件偏移;bptoffset:pBufferPtr偏移)
	int32_t TcpSessionWriteFile(uint64_t offset,
		std::shared_ptr<std::vector<char>> &pBufferPtr,	std::size_t size,
		std::shared_ptr<tagTcpSessionFileParam> &pTcpSessionFileParamPtr, 
		std::size_t bptoffset = 0);

public:

	/// 同步读取数据
	std::size_t SyncReadFile(uint64_t offset, std::shared_ptr<std::vector<char>> &pBufferPtr,
		std::size_t size, std::error_code& err, std::size_t bptoffset = 0);

	/// 同步写入数据
	std::size_t SyncWriteFile(uint64_t offset, std::shared_ptr<std::vector<char>> &pBufferPtr,
		std::size_t size, std::error_code& err, std::size_t bptoffset = 0);

public:

	/// 异步推送理事件
	using AsyncPostHandleFun = std::function<void()>;
	uint32_t AsyncPostHandle(AsyncPostHandleFun handle);

	/// 获取SERVICE POOL(需捕获system_error异常)
	std::shared_ptr<IoServicePool>& IoFile::GetIoServicePool();

public:

	/// 获取文件大小
	int64_t GetFileSize() { return m_nFileSize.QuadPart; }

	/// 获取文件句柄
	HANDLE GetFileHandle() { return m_hFile; }

private:

	IoFileImpPtr m_pIoFileImpPtr;

	HANDLE m_hFile;

	LARGE_INTEGER m_nFileSize;			// 文件大小(单位：字节)

	bool m_bStop;
};

END_LIBNETIO_NAME_SPACE

#endif

