#ifndef __NETWORK_SESSION_TIMEOUT_HPP
#define __NETWORK_SESSION_TIMEOUT_HPP

#include "../config.hpp"


namespace network {

	using close_session_handler_t = std::function<void(const session_ptr &)>;

	struct session_entry_t
	{
		session_weak_ptr session_weak_;
		const close_session_handler_t &close_handler_;

		session_entry_t(const session_ptr &, const close_session_handler_t &);
		~session_entry_t();
	};
	using session_entry_ptr = std::shared_ptr<session_entry_t>; 

	struct session_timeout_t
	{
		struct impl;
		std::unique_ptr<impl> impl_;

		explicit session_timeout_t(const error_handler_t &);
		~session_timeout_t();

		session_timeout_t(const session_timeout_t &) = delete;
		session_timeout_t &operator=(const session_timeout_t &) = delete;

		bool start(std::chrono::seconds);
		void stop();

		void update_session(const session_entry_ptr &);
	};
}


#endif