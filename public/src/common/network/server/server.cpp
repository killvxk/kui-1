#include "server.hpp"

#include <asio.hpp>
#include <atomic>

#include "service_pool.hpp"


namespace network {

	using namespace asio::ip;

	struct session_t
		: std::enable_shared_from_this<session_t>
	{
		const error_handler_t &error_handler_;
		const msg_size_handler_t &msg_size_handler_;

		const server_t::session_msg_handler_t &msg_handler_;
		const server_t::session_disconnected_handler_t &disconnected_handler_;

		asio::io_service &io_;
		tcp::socket socket_;

		void *ctx_ = nullptr;
		std::uint32_t header_size_ = 0;
		buffer_t recv_buffer_ = make_buffer(header_size_);

		std::string remote_ip_;
		std::string locale_ip_;

		session_t(const error_handler_t &error_handler, asio::io_service &io, 
			const header_size_handler_t &header_size_handler, 
			const msg_size_handler_t &msg_size_handler,
			const server_t::session_msg_handler_t &session_handler,
			const server_t::session_disconnected_handler_t &disconnected_handler)
			: error_handler_(error_handler)
			, msg_size_handler_(msg_size_handler)
			, msg_handler_(session_handler)
			, disconnected_handler_(disconnected_handler)
			, io_(io)
			, socket_(io_)
			, header_size_(header_size_handler())
		{
		}

		~session_t()
		{
		}

		void start()
		{
			asio::error_code ec;
			socket_.set_option(asio::socket_base::linger(true, 30), ec);
			socket_.set_option(asio::ip::tcp::no_delay(true), ec);
			socket_.set_option(asio::socket_base::keep_alive(true), ec);

			remote_ip_ = socket_.remote_endpoint(ec).address().to_string();
			locale_ip_ = socket_.local_endpoint(ec).address().to_string();

			if( ec )
				error_handler_(ec);
		}

		void async_read()
		{
			auto shared_this = shared_from_this();
		
			asio::async_read(socket_, asio::buffer(recv_buffer_->data(), header_size_),
				[shared_this, this](const std::error_code &err, std::size_t size)
			{
				if( err )
				{
					if( size != 0 )
						error_handler_(err);

					handle_stop();
					return;
				}

				using namespace std::placeholders;
				auto data = recv_buffer_->data();
				auto msg_length = msg_size_handler_(data);

				if( msg_length == 0 )
				{
					handle_stop();
					return;
				}

				if( msg_length > recv_buffer_->size() )
					recv_buffer_->resize(msg_length);

				if( msg_length == size )
				{
					msg_handler_(shared_from_this(), std::cref(recv_buffer_));
					async_read();
				}
				else
				{
					asio::async_read(socket_, asio::buffer(recv_buffer_->data() + header_size_, msg_length - header_size_),
						std::bind(&session_t::handle_body, shared_from_this(), _1, _2));
				}
			});
		}

		void handle_body(const std::error_code &err, std::size_t size)
		{
			if( err )
			{
				if( size != 0 )
					error_handler_(err);

				handle_stop();
				return;
			}

			msg_handler_(shared_from_this(), std::cref(recv_buffer_));
				
			async_read();
		}

		void async_send(const buffer_t &buffer)
		{
			auto shared_this = shared_from_this();
			
			asio::async_write(socket_, asio::buffer(buffer->data(), buffer->size()), 
				[shared_this, buffer](const std::error_code &err, std::size_t size)
			{
				auto tmp = buffer;
				if( !err )
					assert(size == buffer->size());
			});
		}

		void handle_stop()
		{
			std::error_code err;
			socket_.shutdown(asio::socket_base::shutdown_both, err);
			socket_.close(err);
			disconnected_handler_(shared_from_this());
		}

		void *context() const
		{
			return ctx_;
		}

		void context(void *ctx)
		{
			ctx_ = ctx;
		}
	};


	const std::string &session_remote_ip(const session_ptr &session)
	{
		return session->remote_ip_;
	}

	const std::string &session_local_ip(const session_ptr &session)
	{
		return session->locale_ip_;
	}

	void shutdown(const session_ptr &session)
	{
		session->handle_stop();
	}

	void session_context(const session_ptr &session, void *ctx)
	{
		session->context(ctx);
	}

	void *session_context(const session_ptr &session)
	{
		return session->context();
	}

	void send_msg(const session_ptr &session, const buffer_t &buffer)
	{
		session->async_send(buffer);
	}


	struct server_t::impl
	{
		error_handler_t error_handler_;
		header_size_handler_t header_size_handler_;
		msg_size_handler_t msg_size_handler_;

		io_service_pool_t service_pool_;
		std::unique_ptr<tcp::acceptor> acceptor_;

		session_connected_handler_t connected_handler_;
		session_disconnected_handler_t disconnected_handler_;
		session_msg_handler_t msg_handler_;
		
		std::atomic<bool> is_exit_ = false;

		impl(const error_handler_t &error_handler, const header_size_handler_t &header_size_handler, const msg_size_handler_t &msg_size_handler)
			: error_handler_(error_handler)
			, header_size_handler_(header_size_handler)
			, msg_size_handler_(msg_size_handler)
			, service_pool_(error_handler_)
		{}
		~impl()
		{

		}

		bool start(std::uint16_t port, std::uint8_t thr_cnt)
		{
			try
			{
				service_pool_.start(thr_cnt);

				acceptor_ = std::make_unique<tcp::acceptor>(service_pool_.get_io_service(), tcp::endpoint(tcp::v4(), port));
				start_accept();
			}
			catch( const std::system_error &e )
			{
				error_handler_(e.code());
				return false;
			}

			return true;
		}

		void stop()
		{
			try
			{
				is_exit_ = true;

				if( acceptor_ )
				{
					acceptor_->close();
					acceptor_.reset();
				}

				service_pool_.stop();
			}
			catch( const std::system_error &e )
			{
				error_handler_(e.code());
			}

		}

		void start_accept()
		{
			if( is_exit_ )
				return;

			auto session = std::make_shared<session_t>(error_handler_, service_pool_.get_io_service(), 
				header_size_handler_, msg_size_handler_, msg_handler_, disconnected_handler_);

			acceptor_->async_accept(session->socket_, [this, session](const std::error_code &err)
			{
				if( !err )
				{
					session->start();
					connected_handler_(session);
					session->async_read();
				}
				else if (!is_exit_)
					error_handler_(err);

				start_accept();
			});
		}
	};


	server_t::server_t(const error_handler_t &error_handler, const header_size_handler_t &header_size_handler, const msg_size_handler_t &msg_size_handler)
		: impl_(std::make_unique<impl>(error_handler, header_size_handler, msg_size_handler))
	{

	}
	server_t::~server_t()
	{}


	bool server_t::start(std::uint16_t port, std::uint8_t thr_cnt)
	{
		return impl_->start(port, thr_cnt);
	}

	void server_t::stop()
	{
		impl_->stop();
	}

	void server_t::reg_status_handler(const session_connected_handler_t &connected_handler, const session_disconnected_handler_t &disconnected_handler)
	{
		impl_->connected_handler_ = connected_handler;
		impl_->disconnected_handler_ = disconnected_handler;
	}

	void server_t::reg_msg_handler(const session_msg_handler_t &session_handler)
	{
		impl_->msg_handler_ = session_handler;
	}

}