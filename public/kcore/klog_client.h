#pragma once
#include "public.h"
#include <functional>

START_NAMESPACE_KCORE

using Log_Handler_Fun = std::function<void(LogLevel, const std::wstring &)>;

class Cklog_client
{
public:
	Cklog_client(Log_Handler_Fun log_out);
	~Cklog_client();

	bool start(const std::wstring &ip, USHORT port);
	void stop();
	void log(LogLevel level, const std::wstring &msg);
private:
	SOCKET	m_socket;
	Log_Handler_Fun m_Out_Loghandle_Fun;
};

END_NAMESPACE_KCORE