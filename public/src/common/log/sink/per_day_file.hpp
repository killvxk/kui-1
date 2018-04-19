#ifndef __LOG_SINK_PER_DAY_FILE_HPP
#define __LOG_SINK_PER_DAY_FILE_HPP

#include "../details/log_impl.hpp"

#include <memory>

namespace task {
	struct timing_task_t;
}

namespace logger {
	struct log_t;
}

namespace logger { namespace sink {

	struct per_day_file_t
	{
		struct impl;
		std::unique_ptr<impl> impl_;

		per_day_file_t(log_t &, task::timing_task_t &, const std::string &);
		~per_day_file_t();

		void write(const std::string &);
		void flush();
	};

}

}


#endif