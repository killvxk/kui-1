#pragma once

#include <stdint.h>
#include <memory>
#include <system_error>
#include <vector>
#include <functional>

#ifndef START_LIBNETIO_NAME_SPACE
#define START_LIBNETIO_NAME_SPACE	namespace libnetio {
#endif
#ifndef END_LIBNETIO_NAME_SPACE
#define END_LIBNETIO_NAME_SPACE		}
#endif

START_LIBNETIO_NAME_SPACE

// =======================================================
class IoServicePool;
struct TcpSession;
class IoTcpServer
{
public:

	struct tagIoServerImp;
	using IoServerImpPtr = std::unique_ptr<tagIoServerImp>;

public:

	IoTcpServer();

	virtual ~IoTcpServer();

public:

	/// 开启服务器
	virtual int32_t Start(uint16_t ui16Port, uint32_t ui32LocalIp = 0,
		uint32_t uiTimeOut = 0, std::size_t iThreadCount = 0, bool bReuseAddr = false);

	/// 开启服务器
	virtual int32_t Start(uint16_t ui16Port, std::shared_ptr<IoServicePool> &pIoServicePoolPtr,
		uint32_t uiTimeOut = 0, uint32_t ui32LocalIp = 0, bool bReuseAddr = false);

	/// 停止服务器
	virtual void Stop();

	/// 输出日志
	virtual void WriteLogInfo(const std::error_code &err);

public:

	/// 获取TCP监听端口
	inline uint16_t GetPort() { return m_ui16Port; }

	/// 获取TCP监听绑定的IP
	inline uint32_t GetIp() { return m_ui32LocalIp; }

	/// 获取TCP监听绑定的线程数量
	inline std::size_t GetThreadCount() { return m_iThreadCount; }

public:

	/// 接收客户机连接响应
	virtual void OnAccept(const std::shared_ptr<TcpSession> &pSessionPtr) = 0;

	/// 接收到数据响应
	virtual void OnReceive(const std::shared_ptr<TcpSession> &pSessionPtr,
		std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t offset, std::size_t size) = 0;

	/// 发送数据后响应
	virtual void OnSend(const std::shared_ptr<TcpSession> &pSessionPtr, 
		std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t offset, std::size_t size) = 0;

	/// 客户机关闭后响应
	virtual void OnClose(const std::shared_ptr<TcpSession> &pSessionPtr) = 0;

public:

	/// 主动发送数据(isall:<true:size必须发送完成;false:size不需要发送完成)
	void SendBuffer(const std::shared_ptr<TcpSession> &pSessionPtr, 
		std::shared_ptr<std::vector<char>>& pBufferPtr, 
		std::size_t size, std::size_t offset = 0, bool isall = true);

	/// 异步接收数据(isall:<true:size必须接收完成;false:size不需要接收完成)
	void ReceiveBuffer(const std::shared_ptr<TcpSession> &pSessionPtr,
		std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t size, std::size_t offset = 0, bool isall = true);

	/// 主动关闭连接
	void Close(const std::shared_ptr<TcpSession> &pSessionPtr);

	/// 主动关闭所有连接
	void CloseAll();

public:

	/// 主动同步发送数据
	std::size_t SyncSendBuffer(const std::shared_ptr<TcpSession> &pSessionPtr,
		std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t size, std::error_code &err, std::size_t offset = 0);

	/// 主动同步接收数据
	std::size_t SyncRecvBuffer(const std::shared_ptr<TcpSession> &pSessionPtr,
		std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t size, std::error_code &err, std::size_t offset = 0);

public:

	/// 异步推送理客户机事件
	using AsyncPostHandleTcpSessionFun = std::function<void()>;
	uint32_t AsyncPostHandle(const std::shared_ptr<TcpSession> &pSessionPtr, 
		AsyncPostHandleTcpSessionFun handle);

public:

	/// 异步推送理事件
	using AsyncPostHandleFun = std::function<void()>;
	uint32_t AsyncPostHandle(AsyncPostHandleFun handle);

	/// 获取SERVICE POOL(需捕获system_error异常)
	std::shared_ptr<IoServicePool>& GetIoServicePool();

public: /// 获取客户机属性

	/// 获取session id
	uint64_t GetSessionId(const std::shared_ptr<TcpSession> &pSessionPtr);

	/// 获取远程IP
	uint32_t GetPeerIp(const std::shared_ptr<TcpSession> &pSessionPtr);

	/// 获取本地IP
	uint32_t GetLocalIp(const std::shared_ptr<TcpSession> &pSessionPtr);

	/// 获取SOCKET句柄
	uint64_t GetSocket(const std::shared_ptr<TcpSession> &pSessionPtr);

	/// 设置接收缓冲区
	void SetRecvBuffer(const std::shared_ptr<TcpSession> &pSessionPtr,
		const std::shared_ptr<std::vector<char>> &pRecvBufferPtr);

	/// 设置发送缓冲区
	void SetSendBuffer(const std::shared_ptr<TcpSession> &pSessionPtr,
		const std::shared_ptr<std::vector<char>> &pSendBufferPtr);

	/// 获取接收缓冲区
	std::shared_ptr<std::vector<char>> &GetRecvBuffer(const std::shared_ptr<TcpSession> &pSessionPtr);

	/// 获取发送缓冲区
	std::shared_ptr<std::vector<char>> &GetSendBuffer(const std::shared_ptr<TcpSession> &pSessionPtr);

	/// 设置接收缓冲区可接收大小
	void SetRecvBufferSize(const std::shared_ptr<TcpSession> &pSessionPtr, std::size_t size);

	/// 获取接收缓冲区可接收大小
	std::size_t GetRecvBufferSize(const std::shared_ptr<TcpSession> &pSessionPtr);

private:

	uint16_t m_ui16Port;
	uint32_t m_ui32LocalIp;
	std::size_t m_iThreadCount;
	IoServerImpPtr m_pIoServerImpPtr;
	bool m_bStop;
};

END_LIBNETIO_NAME_SPACE