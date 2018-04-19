#ifndef __LOG_HPP
#define __LOG_HPP

#include <cstdint>
#include <memory>
#include <functional>
#include <vector>

#include "details/log_impl.hpp"


namespace logger {

	
	using buffer_t = std::shared_ptr<std::vector<char>>;
	using sink_handler_t = std::function<void(const std::string &)>;

	
	struct log_t
	{
		struct impl;
		std::unique_ptr<impl> impl_;

		log_t();
		~log_t();

		std::uint32_t add_sink(const sink_handler_t &);
		void erase_sink(std::uint32_t);

		void write(level_t, const std::string &);
	};
}


#endif