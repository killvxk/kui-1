#pragma once
#include "kcore.h"
#include <queue>
using namespace std;

START_NAMESPACE_KCORE

namespace log {

	enum LogLevel
	{
		trace,
		debug,
		info,
		notice,
		warn,
		err,
		critical,
	};
	inline const wchar_t* to_wstr(LogLevel l)
	{
		static const wchar_t* level_names[]
		{
			L"trace", L"debug", L"info", L"notice", L"warning", L"error", L"critical",
		};

		return level_names[(DWORD)l];
	}

	inline const char* to_str(LogLevel l)
	{
		static const char* level_names[]
		{
			"trace", "debug", "info", "notice", "warning", "error", "critical",
		};

		return level_names[(DWORD)l];
	}

}
using namespace log;

struct LogHandle
{
	virtual void AddId(DWORD_PTR id) = 0;
	virtual void EraseId(DWORD_PTR id) = 0;
	virtual void Write(DWORD_PTR id, LogLevel level, TCHAR* pMsg) = 0;
	virtual void Flush() = 0;
	virtual void Release() = 0;
};



END_NAMESPACE_KCORE