#include "sys_console.hpp"
#include "../log.hpp"

#include <iostream>

namespace logger { namespace sink { 

	sys_console_t::sys_console_t(log_t &log)
	{
		log.add_sink(std::bind(&sys_console_t::write, this, std::placeholders::_1));
	}
	
	void sys_console_t::write(const std::string &msg)
	{
		std::cout << msg;
	}
		
	void sys_console_t::flush()
	{

	}

} }