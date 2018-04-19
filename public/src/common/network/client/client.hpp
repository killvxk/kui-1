#ifndef __NETWORK_CLIENT_HPP
#define __NETWORK_CLIENT_HPP

#include "../config.hpp"
#include "../buffer.hpp"

namespace network {

	class client_t
	{
	public:
		using msg_handler_t = std::function<void(const buffer_t &)>;
		using heartbeat_handler_t = std::function<buffer_t()>;

		struct impl;
		std::unique_ptr<impl> impl_;

	public:
		client_t(const error_handler_t &, const header_size_handler_t &, const msg_size_handler_t &);
		~client_t();

		client_t(const client_t &) = delete;
		client_t &operator=(const client_t &) = delete;

		bool start(const std::string &, std::uint16_t, const msg_handler_t &, std::chrono::system_clock::duration);
		void stop();

		buffer_t send(const buffer_t &, std::chrono::system_clock::duration);
		void async_send(const buffer_t &, const msg_handler_t &);

		void set_heartbeat(std::chrono::seconds, const heartbeat_handler_t &);
		std::string local_ip() const;
	};

	namespace details {

		// ---------
		struct normal_client_impl_t
			: client_t
		{
			normal_client_impl_t(const error_handler_t &error_handler, 
				const header_size_handler_t &header_size_handler, 
				const msg_size_handler_t &msg_size_handler)
				: client_t(error_handler, header_size_handler, msg_size_handler)
			{}

			bool start(const std::string &ip, std::uint16_t port, const msg_handler_t &msg_handler, std::chrono::system_clock::duration duration)
			{
				return client_t::start(ip, port, msg_handler, duration);
			}

			void async_send(const buffer_t &buffer)
			{
				client_t::async_send(buffer, nullptr);
			}

			buffer_t send(const buffer_t &, std::chrono::system_clock::duration) = delete;
			void async_send(const buffer_t &, const msg_handler_t &) = delete;
		};


		// -------------
		struct response_client_impl_t
			: client_t
		{
			response_client_impl_t(const error_handler_t &error_handler, 
				const header_size_handler_t &header_size_handler, 
				const msg_size_handler_t &msg_size_handler)
				: client_t(error_handler, header_size_handler, msg_size_handler)
			{}

			bool start(const std::string &, std::uint16_t, const msg_handler_t &, std::chrono::system_clock::duration) = delete;

			bool start(const std::string &ip, std::uint16_t port, std::chrono::system_clock::duration duration)
			{
				return client_t::start(ip, port, nullptr, duration);
			}

			void async_send(const buffer_t &, const msg_handler_t &) = delete;
		};

	}

	using normal_client_t = details::normal_client_impl_t;
	using response_client_t = details::response_client_impl_t;

}



#endif