#pragma once

#include <stdint.h>
#include <memory>
#include <functional>

namespace asio
{
	class io_service;
}

#ifndef START_LIBNETIO_NAME_SPACE
#define START_LIBNETIO_NAME_SPACE	namespace libnetio {
#endif
#ifndef END_LIBNETIO_NAME_SPACE
#define END_LIBNETIO_NAME_SPACE		}
#endif

START_LIBNETIO_NAME_SPACE

// =======================================================
class IoServicePool
{
public:

	struct tagIoServicePoolImp;
	using IoServicePoolImpPtr = std::unique_ptr<tagIoServicePoolImp>;

public:

	IoServicePool();

	~IoServicePool();

public:

	/// 开启服务池
	uint32_t Start(std::size_t iServiceNumber = 0, std::size_t iThreadNumberByService = 1);

	/// 停止服务池
	void Stop();

	/// 获取单个SERVICE(需捕获system_error异常)
	std::shared_ptr<asio::io_service>& GetIoService();

public:

	/// 异步推送理事件
	using AsyncPostHandleFun = std::function<void()>;
	uint32_t AsyncPostHandle(AsyncPostHandleFun handle);

private:

	IoServicePoolImpPtr m_pIoServicePoolImpPtr;

	bool m_bStop;

};

END_LIBNETIO_NAME_SPACE