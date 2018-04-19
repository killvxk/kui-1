#ifndef __NETWORK_SERVER_HPP
#define __NETWORK_SERVER_HPP


#include "../config.hpp"
#include "../buffer.hpp"


namespace network { 


	struct server_t
	{
		struct impl;
		std::unique_ptr<impl> impl_;

		using session_msg_handler_t = std::function<void(const session_ptr &, const buffer_t &)>;
		using session_connected_handler_t = std::function<void(const session_ptr &)>;
		using session_disconnected_handler_t = std::function<void(const session_ptr &)>;

		server_t(const error_handler_t &, const header_size_handler_t &, const msg_size_handler_t &);
		~server_t();

		server_t(const server_t &) = delete;
		server_t &operator=(const server_t &) = delete;

		bool start(std::uint16_t port, std::uint8_t thr_cnt);
		void stop();

		void reg_status_handler(const session_connected_handler_t &, const session_disconnected_handler_t &);
		void reg_msg_handler(const session_msg_handler_t &);
	};


	const std::string &session_remote_ip(const session_ptr &);
	const std::string &session_local_ip(const session_ptr &);

	void shutdown(const session_ptr &);

	void session_context(const session_ptr &, void *);
	void *session_context(const session_ptr &);

	void send_msg(const session_ptr &, const buffer_t &);
}

#endif