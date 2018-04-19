#pragma once

#include <memory>
#include <functional>
#include <system_error>
#include <chrono>

#ifndef START_LIBNETIO_NAME_SPACE
#define START_LIBNETIO_NAME_SPACE	namespace libnetio {
#endif
#ifndef END_LIBNETIO_NAME_SPACE
#define END_LIBNETIO_NAME_SPACE		}
#endif

START_LIBNETIO_NAME_SPACE

class IoServicePool;

class IoTimer
{
public:

	struct tagIoTimerImp;
	using IoTimerImpPtr = std::unique_ptr<tagIoTimerImp>;
	using AsyncTimeOutFun = std::function<void()>;

public:

	IoTimer();

	virtual ~IoTimer();

public:

	/// 开启服务器(iDelayExcuteTime:单位:毫秒)
	virtual int32_t Start(AsyncTimeOutFun pAsyncTimeOutFun, 
		AsyncTimeOutFun pAsyncCancelFun, int32_t iDelayExcuteTime = 0,
		std::size_t iThreadCount = 0);

	/// 开启服务器(iDelayExcuteTime:单位:毫秒)
	virtual int32_t Start(std::shared_ptr<IoServicePool> &pIoServicePoolPtr, 
		AsyncTimeOutFun pAsyncTimeOutFun, AsyncTimeOutFun pAsyncCancelFun, 
		int32_t iDelayExcuteTime = 0);

	/// 开启服务器
	virtual int32_t Start(AsyncTimeOutFun pAsyncTimeOutFun,
		AsyncTimeOutFun pAsyncCancelFun, const std::chrono::steady_clock::time_point &tTimePoint,
		std::size_t iThreadCount = 0);

	/// 开启服务器
	virtual int32_t Start(std::shared_ptr<IoServicePool> &pIoServicePoolPtr,
		AsyncTimeOutFun pAsyncTimeOutFun, AsyncTimeOutFun pAsyncCancelFun,
		const std::chrono::steady_clock::time_point &tTimePoint);

	/// 停止服务器
	virtual void Stop();

	/// 输出日志
	virtual void WriteLogInfo(const std::error_code &err);

public:

	/// 异步推送理事件
	using AsyncPostHandleFun = std::function<void()>;
	uint32_t AsyncPostHandle(AsyncPostHandleFun handle);

	/// 获取SERVICE POOL(需捕获system_error异常)
	std::shared_ptr<IoServicePool>& GetIoServicePool();

public:

	/// 取消时钟
	void CancelTimer();

	/// 重置时钟(iDelayExcuteTime:单位:毫秒)
	void ResetTimer(AsyncTimeOutFun pAsyncTimeOutFun,
		AsyncTimeOutFun pAsyncCancelFun, int32_t iDelayExcuteTime = 0);

private:

	IoTimerImpPtr m_pIoTimerImpPtr;
	bool m_bStop;

};

END_LIBNETIO_NAME_SPACE