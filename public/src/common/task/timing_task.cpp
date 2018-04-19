#include "timing_task.hpp"

#define ASIO_STANDALONE
#include <asio.hpp>

#include <thread>
#include <container/sync_container.hpp>


namespace task {

	struct timer_handle_t
	{
		std::unique_ptr<asio::steady_timer> timer_;
	};

	struct timing_task_t::impl
	{
		asio::io_service io_;
		std::unique_ptr<std::thread> thr_;
		stdex::container::sync_assoc_container_t<timer_ptr, timer_ptr> timers_;

		timer_ptr add(std::chrono::system_clock::time_point t,
			std::chrono::system_clock::duration duration,
			const std::function<void()> &handler)
		{
			auto steady_timer = std::make_unique<asio::steady_timer>(io_);
			auto timer = std::make_shared<timer_handle_t>();
			timer->timer_ = std::move(steady_timer);
			
			using namespace std::placeholders;
			std::error_code err;
			timer->timer_->expires_at(t, err);
			timer->timer_->async_wait(std::bind(&impl::on_timer, this, _1, timer, duration, handler));

			timers_.insert(timer, timer);
			return timer;
		}

		void erase(const timer_ptr &timer)
		{
			std::error_code err;
			timer->timer_->cancel(err);
			timers_.erase(timer);
		}

		void on_timer(std::error_code err, 
			const timer_ptr &timer, 
			std::chrono::system_clock::duration duration, 
			const std::function<void()> &handler)
		{
			if( err )
				return;

			using namespace std::placeholders;

			handler();

			timer->timer_->expires_from_now(duration, err);
			timer->timer_->async_wait(std::bind(&impl::on_timer, this, _1, timer, duration, handler));
		}
	};

	timing_task_t::timing_task_t()
		: impl_(std::make_unique<impl>())
	{}

	timing_task_t::~timing_task_t()
	{}

	bool timing_task_t::start()
	{
		impl_->thr_ = std::make_unique<std::thread>([this]()
		{
			asio::io_service::work work(impl_->io_);

			std::error_code err;
			impl_->io_.run(err);
		});
			
		return true;
	}

	void timing_task_t::stop()
	{
		if( !impl_->thr_ )
			return;

		impl_->timers_.for_each([](const std::pair<timer_ptr, timer_ptr> &v) 
		{
			std::error_code err;
			v.second->timer_->cancel(err);
		});
		impl_->timers_.clear();

		impl_->io_.stop();
		impl_->thr_->join();
	}

	timer_ptr timing_task_t::add(std::chrono::system_clock::time_point t, 
		std::chrono::system_clock::duration duration, 
		const std::function<void()> &handler)
	{
		return impl_->add(t, duration, handler);
	}

	void timing_task_t::erase(const timer_ptr &t)
	{
		impl_->erase(t);
	}
}