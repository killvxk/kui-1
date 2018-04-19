#include "time.hpp"

#include <sstream>
#include <locale>
#include <iomanip>
#include <iostream>
#include <codecvt>
#include <ctime>

namespace sys_time {

	std::chrono::system_clock::time_point to_time(const std::wstring &val)
	{
		std::tm t = {0};
		std::wistringstream ss(val);
		ss >> std::get_time(&t, L"%Y-%m-%d %H:%M:%S");

		return std::chrono::system_clock::from_time_t(std::mktime(&t));
	}


	std::wstring to_string(const std::chrono::time_point<std::chrono::system_clock> &time_point)
	{
		std::wstringstream os;
		auto t = std::chrono::system_clock::to_time_t(time_point);

		std::tm tm = {0};
		localtime_s(&tm, &t);
		os << std::put_time(&tm, L"%Y-%m-%d %H:%M:%S");
		return os.str();
	}

	std::wstring now()
	{
		return to_string(std::chrono::system_clock::now());
	}
}