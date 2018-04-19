#ifndef __NETWORK_SERVICE_POOL_HPP
#define __NETWORK_SERVICE_POOL_HPP


#include "../config.hpp"


namespace asio {
	class io_service;
}

namespace network {

	struct io_service_pool_t
	{
		struct impl;
		std::unique_ptr<impl> impl_;

		io_service_pool_t(const error_handler_t &);
		~io_service_pool_t();

		io_service_pool_t(const io_service_pool_t &) = delete;
		io_service_pool_t &operator=(const io_service_pool_t &) = delete;

		void start(std::size_t);
		void stop();

		asio::io_service& get_io_service();
	};
}

#endif 
