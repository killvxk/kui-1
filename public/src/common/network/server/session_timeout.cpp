#include "session_timeout.hpp"

#include <utility\timewheel.hpp>
#include <asio.hpp>

#include "service_pool.hpp"

namespace network {

	
	session_entry_t::session_entry_t(const session_ptr &session, const close_session_handler_t &close_handler)
		: session_weak_(session)
		, close_handler_(close_handler)
	{}

	session_entry_t::~session_entry_t()
	{
		auto session = session_weak_.lock();
		if( session )
			close_handler_(std::cref(session));
	}

	struct session_timeout_t::impl
	{
		io_service_pool_t io_;
		std::unique_ptr<asio::steady_timer> timer_;
		
		close_session_handler_t close_handler_;
		utility::timewheel_t<
			session_entry_t, 
			4
		> session_timewheel_;


		impl(const error_handler_t &error_handler)
			: io_(error_handler)
		{}

		void set_timer(std::chrono::seconds expired)
		{
			timer_->expires_from_now(expired);
			timer_->async_wait([this, expired](const std::error_code&)
			{
				handle_timer();

				set_timer(expired);
			});
		}

		void handle_timer()
		{
			session_timewheel_.push_new_entry();
		}
	};


	session_timeout_t::session_timeout_t(const error_handler_t &error_handler)
		: impl_(std::make_unique<impl>(error_handler))
	{}

	session_timeout_t::~session_timeout_t()
	{}


	bool session_timeout_t::start(std::chrono::seconds expired)
	{
		impl_->io_.start(1);

		impl_->timer_ = std::make_unique<asio::steady_timer>(impl_->io_.get_io_service(), expired);
		impl_->set_timer(expired);

		return true;
	}

	void session_timeout_t::stop()
	{
		impl_->timer_->cancel();
		impl_->io_.stop();
	}

	void session_timeout_t::update_session(const session_entry_ptr &session)
	{
		impl_->session_timewheel_.update_entry(session);
	}
}