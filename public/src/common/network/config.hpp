#ifndef __NETWORK_CONFIG_HPP
#define __NETWORK_CONFIG_HPP

#include <cstdint>
#include <cassert>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <thread>

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif

#ifdef _MSC_VER
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0X0501
#endif
#endif

namespace network {

	using buffer_t = std::shared_ptr<std::vector<char>>;
		
	struct session_t;
	using session_ptr = std::shared_ptr<session_t>;
	using session_weak_ptr = std::weak_ptr<session_t>;

	using error_handler_t = std::function<void(const std::error_code &)>;

	using header_size_handler_t = std::function<std::uint32_t()>;
	using msg_size_handler_t = std::function<std::uint32_t(const char *)>;
}

#endif