#ifndef __LOG_SINK_SYS_FILE_HPP
#define __LOG_SINK_SYS_FILE_HPP

#include "../details/log_impl.hpp"


namespace logger {
	struct log_t;
}

namespace logger { namespace sink {

	struct sys_console_t
	{
		sys_console_t(log_t &);

		void write(const std::string &);
		void flush();
	};
}

}


#endif