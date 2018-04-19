#include "client.hpp"

#include <asio.hpp>
#include <future>


namespace network {

	using asio::ip::tcp;


	struct heartbeat_t
	{
		asio::steady_timer timer_;

		heartbeat_t(asio::io_service &io)
			: timer_(io)
		{}

		template < typename HandlerT >
		void start(std::chrono::seconds s, HandlerT handler)
		{
			timer_.expires_from_now(s);
			timer_.async_wait([this, s, handler](const std::error_code &err) 
			{
				handler();
				start(s, handler);
			});
		}

		void stop()
		{
			timer_.cancel();
		}
	};

	struct client_t::impl
	{
		error_handler_t error_handler_;
		msg_size_handler_t msg_size_handler_;

		asio::io_service io_;
		tcp::socket socket_;

		std::string ip_;
		std::uint16_t port_ = 0;

		std::uint32_t header_size_ = 0;
		bool is_stop_ = false;
		std::unique_ptr<std::thread> thr_;

		asio::steady_timer timer_;
		heartbeat_t heartbeat_;

		impl(const error_handler_t &error_handler, const header_size_handler_t &header_size_handler, const msg_size_handler_t &msg_size_handler)
			: error_handler_(error_handler)
			, msg_size_handler_(msg_size_handler)
			, io_(1)
			, socket_(io_)
			, header_size_(header_size_handler())
			, timer_(io_)
			, heartbeat_(io_)
		{

		}

		bool start(const std::string &ip, std::uint16_t port, const msg_handler_t &msg_handler, std::chrono::system_clock::duration duration)
		{
			ip_ = ip;
			port_ = port;

			
			auto timer = std::make_shared<asio::steady_timer>(io_);
			timer->expires_from_now(duration);

			std::error_code err = std::make_error_code(std::errc::operation_would_block);
			socket_.async_connect({asio::ip::address::from_string(ip, err), port}, [&err, timer](const std::error_code &e)
			{
				err = e;

				std::error_code ec;
				timer->cancel(ec);
			});
			timer->async_wait([this](const std::error_code &e) 
			{
				if( e )
					return;

				std::error_code ec;
				socket_.close(ec);
			});

			do io_.run_one(); 
			while( err == std::errc::operation_would_block );

			if( err || !socket_.is_open() )
			{
				error_handler_(err);
				return false;
			}

			thr_ = std::make_unique<std::thread>([this]()
			{
				asio::io_service::work work(io_);
				std::error_code err;
				io_.run(err);

				if( err )
					error_handler_(err);
			});


			if( msg_handler != nullptr )
			{
				async_normal_read(msg_handler);
			}

			return true;
		}

		void stop()
		{
			is_stop_ = true;
			heartbeat_.stop();

			std::error_code err;
			socket_.close(err);
			io_.stop();

			if( thr_ )
				thr_->join();
		}

		buffer_t write(const buffer_t &buffer, std::chrono::system_clock::duration s)
		{
			auto promise = std::make_shared<std::promise<buffer_t>>();
			auto future_promise = promise->get_future();

			async_write(buffer, [this, promise](const buffer_t &buffer) 
			{
				promise->set_value(buffer);
			});

			auto ret = future_promise.wait_for(s);
			if( ret == std::future_status::timeout )
				return nullptr;
			else
				return future_promise.get();
		}

		void async_normal_read(const msg_handler_t &msg_handler)
		{
			if( is_stop_ )
				return;

			async_read_impl([this, msg_handler](const buffer_t &buffer)
			{
				msg_handler(std::cref(buffer));

				if( buffer )
					async_normal_read(msg_handler);
			});
		}

		void async_write(const buffer_t &buffer, const msg_handler_t &msg_handler)
		{
			asio::async_write(socket_, asio::buffer(buffer->data(), buffer->size()),
				[this, buffer, msg_handler](const std::error_code &err, std::size_t size)
			{
				if( err )
				{
					error_handler_(err);

					//reconnect_impl();
					return;
				}

				if( msg_handler != nullptr )
					async_read_impl(msg_handler);
			});
		}

		void async_read_impl(const msg_handler_t &msg_handler)
		{
			buffer_t recv_buffer = make_buffer(header_size_);
			asio::async_read(socket_, asio::buffer(recv_buffer->data(), recv_buffer->size()),
				[this, recv_buffer, msg_handler](const std::error_code &err, std::size_t size)
			{
				if( err )
				{
					error_handler_(err);
					msg_handler({});
					return;
				}

				using namespace std::placeholders;
				auto data = recv_buffer->data();
				auto msg_length = msg_size_handler_(data);
				if( msg_length == 0 )
				{
					stop();
					return;
				}

				if( msg_length > recv_buffer->size() )
					recv_buffer->resize(msg_length);

				if( msg_length == size )
				{
					msg_handler(std::cref(recv_buffer));
					return;
				}
				
				asio::async_read(socket_, asio::buffer(recv_buffer->data() + header_size_, msg_length - header_size_),
					[this, recv_buffer, msg_handler](const std::error_code &err, std::size_t size)
				{
					if( err )
					{
						error_handler_(err);
						msg_handler({});
						return;
					}

					msg_handler(std::cref(recv_buffer));
				});
			});
		}

		bool reconnect_impl()
		{
			try
			{
				if( socket_.is_open() )
					socket_.close();

				tcp::resolver resolver(io_);
				auto endpoint_iterator = resolver.resolve({ip_, std::to_string(port_)});
				asio::connect(socket_, endpoint_iterator);
			}
			catch( const std::system_error &e )
			{
				error_handler_(e.code());
				return false;
			}

			return true;
		}

		void check_deadline()
		{
			//if( timer_.expires_at() <= std::chrono::system_clock::now() )
		}
	};

	client_t::client_t(const error_handler_t &error_handler, const header_size_handler_t &header_size_handler, const msg_size_handler_t &msg_size_handler)
		: impl_(std::make_unique<impl>(error_handler, header_size_handler, msg_size_handler))
	{

	}

	client_t::~client_t()
	{

	}

	bool client_t::start(const std::string &ip, std::uint16_t port, 
		const msg_handler_t &msg_handler, std::chrono::system_clock::duration duration)
	{
		return impl_->start(ip, port, msg_handler, duration);
	}

	void client_t::stop()
	{
		impl_->stop();
	}


	buffer_t client_t::send(const buffer_t &buffer, std::chrono::system_clock::duration s)
	{
		return impl_->write(buffer, s);
	}

	void client_t::async_send(const buffer_t &buffer, const msg_handler_t &msg_handler)
	{
		impl_->async_write(buffer, msg_handler);
	}

	void client_t::set_heartbeat(std::chrono::seconds s, const heartbeat_handler_t &handler)
	{
		impl_->heartbeat_.start(s, [this, handler]() 
		{
			auto buffer = handler();
			async_send(buffer, nullptr);
		});
	}

	std::string client_t::local_ip() const
	{
		std::error_code ec;
		return impl_->socket_.local_endpoint(ec).address().to_string();
	}
}