#pragma once

#include <memory>
#include <stdint.h>
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

enum eIoUdpOptType
{
	IoUdpOptTypeStart = 0,
	IoUdpOptTypeStop,
	IoUdpOptTypeSend,
	IoUdpOptTypeRecv,
	IoUdpOptTypePostSend,
	IoUdpOptTypePostRecv,
	IoUdpOptTypeHandle,
};

#pragma pack(push, 1)
struct tagIoUdpSendParamInfo
{
	uint32_t ui32RemotelIp;
	uint16_t ui16Port;
	uint32_t ui32SendSize;
};
#pragma pack(pop)

class IoServicePool;
class IoUdp
{
public:

 	struct tagIoUdpImp;
	using IoUdpImpPtr = std::unique_ptr<tagIoUdpImp>;

public:

	IoUdp();

	virtual ~IoUdp();

public:

	/// 开启服务器
	virtual int32_t Start(uint16_t ui16Port, uint32_t ui32LocalIp = 0,
		std::size_t iThreadCount = 0, bool bReuseAddr = false, bool bBroadcast = false);

	/// 开启服务器
	virtual int32_t Start(uint16_t ui16Port, std::shared_ptr<IoServicePool> &pIoServicePoolPtr,
		uint32_t ui32LocalIp = 0, bool bReuseAddr = false, bool bBroadcast = false);

	/// 停止服务器
	virtual void Stop();

	/// 输出日志
	virtual void WriteLogInfo(eIoUdpOptType type, const std::error_code &err, 
		uint32_t ui32RemotelIp = 0, uint16_t ui16Port = 0);

public:

	/// 接收到数据响应
	virtual void OnReceive(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t offset, std::size_t size, uint32_t ui32RemotelIp, uint16_t ui16Port) = 0;

	/// 发送数据后响应
	virtual void OnSend(std::shared_ptr<std::vector<char>>& pBufferPtr,
		std::size_t offset, std::size_t size, uint32_t ui32RemotelIp, uint16_t ui16Port) = 0;

public:

	/// 获取UDP本地端口
	inline uint16_t GetPort() { return m_ui16Port; }

	/// 获取UDP本地绑定的IP
	inline uint32_t GetIp() { return m_ui32LocalIp; }

public:

	/// 异步推送理事件
	using AsyncPostHandleFun = std::function<void()>;
	uint32_t AsyncPostHandle(AsyncPostHandleFun handle);

	/// 获取SERVICE POOL(需捕获system_error异常)
	std::shared_ptr<IoServicePool>& GetIoServicePool();

	/// 获取SOCKET句柄
	uint64_t GetSocket();

public:

	/// 主动异步发送数据
	void SendBuffer(std::shared_ptr<std::vector<char>>& pBufferPtr,	std::size_t size, 
		uint32_t ui32RemotelIp, uint16_t ui16Port, std::size_t offset = 0);

	/// 异步接收数据
	void ReceiveBuffer(std::shared_ptr<std::vector<char>>& pBufferPtr, std::size_t size,
		std::size_t offset = 0);

public:

	/// 主动异步发送数据(带发送队列, sendparamsize: sizeof(tagIoUdpSendParamInfo))
	void SendBufferEx(std::shared_ptr<std::vector<char>>& pBufferPtr, std::size_t size,
		uint32_t ui32RemotelIp, uint16_t ui16Port, std::size_t sendparamsize);

	/// 异步接收数据(带发送队列, sendparamsize: sizeof(tagIoUdpSendParamInfo))
	void ReceiveBufferEx(std::shared_ptr<std::vector<char>>& pBufferPtr, std::size_t size,
		std::size_t sendparamsize);

public:

	/// 主动发送数据(阻塞发送)
	void SyncSendBuffer(std::shared_ptr<std::vector<char>>& pBufferPtr, std::size_t size,
		uint32_t ui32RemotelIp, uint16_t ui16Port,
		std::size_t offset = 0);

	/// 主动接收数据(阻塞接收)
	void SyncRecvBuffer(std::shared_ptr<std::vector<char>>& pBufferPtr, std::size_t size,
		uint32_t &ui32RemotelIp, uint16_t &ui16Port,
		std::size_t offset = 0);

private:

	IoUdpImpPtr m_pIoUdpImp;
	uint32_t m_ui32LocalIp;
	uint16_t m_ui16Port;
	bool m_bStop;
};

END_LIBNETIO_NAME_SPACE