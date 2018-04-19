#pragma once

#include <memory>
#include <functional>
#include <system_error>
#include <vector>

#ifndef START_LIBNETIO_NAME_SPACE
#define START_LIBNETIO_NAME_SPACE	namespace libnetio {
#endif
#ifndef END_LIBNETIO_NAME_SPACE
#define END_LIBNETIO_NAME_SPACE		}
#endif

START_LIBNETIO_NAME_SPACE

class IoServicePool;

class IoTcpClient
{
public:

	struct tagIoTcpClientImp;
	using IoTcpClientImpPtr = std::unique_ptr<tagIoTcpClientImp>;

public:

	IoTcpClient();

	virtual ~IoTcpClient();

public:

	/// 开启服务器(uiConnectTimeOut:连接超时(单位：毫秒);uiTimeOut:非活跃超时(单位：毫秒))
	virtual int32_t Start(uint32_t uiRemoteIp, uint16_t uiRemotePort, 
		uint32_t uiConnectTimeOut = 0, uint32_t uiTimeOut = 0,
		std::size_t iThreadCount = 0);

	/// 开启服务器
	virtual int32_t Start(std::shared_ptr<IoServicePool> &pIoServicePoolPtr,
		uint32_t uiRemoteIp, uint16_t uiRemotePort,
		uint32_t uiConnectTimeOut = 0, uint32_t uiTimeOut = 0);

	/// 停止服务器
	virtual void Stop();

	/// 输出日志
	virtual void WriteLogInfo(const std::error_code &err);

public:

	/// 客户机连接响应
	virtual void OnConnect() = 0;

	/// 接收到数据响应
	virtual void OnReceive(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t offset, std::size_t size) = 0;

	/// 发送数据后响应
	virtual void OnSend(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t offset, std::size_t size) = 0;

	/// 客户机关闭后响应
	virtual void OnClose() = 0;

public:

	/// 主动发送数据(isall:<true:size必须发送完成;false:size不需要发送完成)
	void SendBuffer(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t size, std::size_t offset = 0, bool isall = true);

	/// 异步接收数据(isall:<true:size必须接收完成;false:size不需要接收完成)
	void ReceiveBuffer(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t size, std::size_t offset = 0, bool isall = true);

	/// 主动关闭连接
	void Close();

public:

	/// 异步推送理事件
	using AsyncPostHandleFun = std::function<void()>;
	uint32_t AsyncPostHandle(AsyncPostHandleFun handle);

	/// 获取SERVICE POOL(需捕获system_error异常)
	std::shared_ptr<IoServicePool>& GetIoServicePool();

public: /// 获取客户机属性

	/// 获取远程IP
	uint32_t GetPeerIp();

	/// 获取本地IP
	uint32_t GetLocalIp();

	/// 获取SOCKET句柄
	uint64_t GetSocket();

	/// 设置接收缓冲区
	void SetRecvBuffer(const std::shared_ptr<std::vector<char>> &pRecvBufferPtr);

	/// 设置发送缓冲区
	void SetSendBuffer(const std::shared_ptr<std::vector<char>> &pSendBufferPtr);

	/// 获取接收缓冲区
	std::shared_ptr<std::vector<char>> &GetRecvBuffer();

	/// 获取发送缓冲区
	std::shared_ptr<std::vector<char>> &GetSendBuffer();

	/// 设置接收缓冲区可接收大小
	void SetRecvBufferSize(std::size_t size);

	/// 获取接收缓冲区可接收大小
	std::size_t GetRecvBufferSize();

public:

	/// 主动同步发送数据
	std::size_t SyncSendBuffer(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t size, std::error_code &err, std::size_t offset = 0);

	/// 主动同步接收数据
	std::size_t SyncRecvBuffer(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t size, std::error_code &err, std::size_t offset = 0);

private:

	IoTcpClientImpPtr m_pIoClientImpPtr;

	bool m_bStop;
};

END_LIBNETIO_NAME_SPACE