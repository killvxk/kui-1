#ifndef __LOG_SINK_SYS_FILE_HPP
#define __LOG_SINK_SYS_FILE_HPP

#include "../details/log_impl.hpp"

#include <memory>
#include <filesystem>

namespace fs = std::tr2::sys;


namespace logger { namespace sink { 

	struct file_t
	{
		struct impl;
		std::unique_ptr<impl> impl_;

		file_t(const fs::path &);
		~file_t();

		void write(const std::string &);
		void flush();
	};
}

}


#endif