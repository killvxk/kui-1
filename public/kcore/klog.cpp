#include "stdafx.h"
#include "klog.h"
#include "klog_server.h"
#include "klog_client.h"
#include <atlbase.h>


START_NAMESPACE_KCORE


// server
bool __stdcall klog_svr_start()
{
	return sklogSvr.StartKlogServer();
}
void __stdcall klog_svr_stop()
{
	sklogSvr.StopKlogServer();
}




//client
klog_client_t __stdcall klog_client_create(sink_t sink, void *param)
{
	return new Cklog_client([param, sink](LogLevel level, const std::wstring &text)
	{
		if (sink != nullptr)
			sink(param, text.c_str(), (unsigned int)text.size());
	});
}

bool __stdcall klog_client_start(klog_client_t handle, const char *ip, unsigned __int16 port)
{
	auto log = (Cklog_client *)handle;

	std::wstring  strIp = CA2W(ip);

	return log->start(strIp, port);
}

void __stdcall klog_client_write(klog_client_t handle, unsigned __int8 level, const wchar_t *text, unsigned __int32 size)
{
	auto log = (Cklog_client *)handle;

	log->log(LogLevel(level), { text, size });
}

void __stdcall klog_client_stop(klog_client_t handle)
{
	auto log = (Cklog_client *)handle;

	log->stop();
}

void __stdcall klog_client_destroy(klog_client_t handle)
{
	auto log = (Cklog_client *)handle;
	delete log;
}

END_NAMESPACE_KCORE