#include "log.hpp"

#include <atomic>

#include "../container/sync_container.hpp"


namespace logger {

	struct log_t::impl
	{
		stdex::container::sync_assoc_container_t<
			std::uint32_t,
			sink_handler_t
		> sink_handlers_;

		std::atomic<std::uint32_t> id_ = 0;
	};

	log_t::log_t()
		: impl_(std::make_unique<impl>())
	{

	}

	log_t::~log_t()
	{

	}

	std::uint32_t log_t::add_sink(const sink_handler_t &handlers)
	{
		++impl_->id_;
		impl_->sink_handlers_.insert(impl_->id_, handlers);

		return impl_->id_;
	}

	void log_t::erase_sink(std::uint32_t id)
	{
		impl_->sink_handlers_.erase(id);
	}

	void log_t::write(level_t level, const std::string &content)
	{
		impl_->sink_handlers_.for_each(
			[level, &content](const std::pair<std::uint32_t, sink_handler_t> &v) 
		{
			v.second(std::cref(content));
		});
	}
}