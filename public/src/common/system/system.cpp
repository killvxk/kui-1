#include "system.hpp"

#include <Windows.h>
#include <Shlwapi.h>

#include <cassert>
#include <atlbase.h>
#include <atlconv.h>

#pragma comment(lib, "shlwapi.lib")

namespace sys {

	namespace details {

		ret_helper_t::ret_helper_t(const wchar_t *buffer)
			: buffer_(buffer)
		{}

		ret_helper_t::operator std::uint8_t() const
		{
			return (std::uint8_t)std::stoul(buffer_);
		}

		ret_helper_t::operator std::uint16_t() const
		{
			return (std::uint16_t)std::stoul(buffer_);
		}

		ret_helper_t::operator std::uint32_t() const
		{
			return std::stoul(buffer_);
		}

		ret_helper_t::operator std::uint64_t() const
		{
			return std::stoull(buffer_);
		}

		ret_helper_t::operator std::string() const
		{
			return (LPCSTR) CW2A(buffer_.c_str(), CP_UTF8);
		}

		ret_helper_t::operator std::wstring() const
		{
			return buffer_;
		}
	}
	
	
	
	std::wstring app_path()
	{
		wchar_t szFileName[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szFileName, _countof(szFileName));
		::PathRemoveFileSpec(szFileName);
		::PathAddBackslash(szFileName);

		return szFileName;
	}


	details::ret_helper_t ini_content(const fs::wpath &path, const std::wstring &section, const std::wstring &key, const std::wstring &defaultvalue)
	{
		auto ini_path = sys::app_path() + path.relative_path().string();
		assert(fs::is_regular_file(fs::wpath(ini_path)));

		wchar_t buffer[MAX_PATH] = { 0 };
		DWORD iret = ::GetPrivateProfileString(section.c_str(), key.c_str(), defaultvalue.c_str(), buffer, _countof(buffer), ini_path.c_str());
		if (iret == 0)
		{
			wcsncpy_s(buffer, defaultvalue.c_str(), min(MAX_PATH - 1, defaultvalue.length()));
		}
			
		return details::ret_helper_t(buffer);
	}
}