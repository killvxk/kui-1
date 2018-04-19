#include "..\time.hpp"

#include <cassert>

#include <string>
#include <sstream>
#include <locale>
#include <iomanip>

void to_time()
{
	std::wstring input = L"2011-10-18 23:12:34";
	std::tm t;
	std::wistringstream ss(input);
	ss >> std::get_time(&t, L"%Y-%m-%d %H:%M:%S");

	auto time1 = std::mktime(&t);
	auto time2 = sys_time::to_time(input);

	assert(time1 == std::chrono::system_clock::to_time_t(time2));
}


void to_string()
{
	std::time_t t = std::time(0);

	std::wostringstream os;
	os << std::put_time(std::localtime(&t), L"%Y-%m-%d %H:%M:%S") << std::endl;

	auto val = sys_time::to_string(std::chrono::system_clock::from_time_t(t));
	auto val1 = os.str();
	assert(val == val1);
}