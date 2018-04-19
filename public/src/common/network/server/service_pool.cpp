#include "service_pool.hpp"

#include <asio.hpp>

namespace network {

	struct io_service_pool_t::impl
	{
		using io_service_ptr = std::shared_ptr<asio::io_service>;
		using work_ptr = std::shared_ptr<asio::io_service::work>;

		error_handler_t error_handler_;

		std::vector<io_service_ptr> io_services_;
		std::vector<work_ptr> work_;
		std::vector<std::shared_ptr<std::thread>> threads_;

		std::size_t next_io_service_ = 0;

		impl(const error_handler_t &error_handler)
			: error_handler_(error_handler)
		{
			
		}
	};
	

	io_service_pool_t::io_service_pool_t(const error_handler_t &error_handler)
		: impl_(std::make_unique<impl>(error_handler))
	{}

	io_service_pool_t::~io_service_pool_t()
	{}


	void io_service_pool_t::start(std::size_t pool_size)
	{
		if( pool_size == 0 )
			pool_size = std::thread::hardware_concurrency();

		impl_->io_services_.reserve(pool_size);
		impl_->work_.reserve(pool_size);
		impl_->threads_.reserve(impl_->io_services_.size());

		for( std::size_t i = 0; i < pool_size; ++i )
		{
			auto io_service(std::make_shared<asio::io_service>());
			auto work(std::make_shared<asio::io_service::work>(*io_service));

			impl_->io_services_.push_back(io_service);
			impl_->work_.push_back(work);

			auto thread(std::make_shared<std::thread>(
				[i, this]()
			{
				std::error_code err;
				impl_->io_services_[i]->run(err);
			}));
			impl_->threads_.push_back(thread);
		}
	}

	void io_service_pool_t::stop()
	{
		for(const auto &v:impl_->io_services_ )
			v->stop();

		for(const auto &v:impl_->threads_ )
			v->join();
	}

	asio::io_service& io_service_pool_t::get_io_service()
	{
		auto &io_service = *impl_->io_services_[impl_->next_io_service_];
		++impl_->next_io_service_;
		if( impl_->next_io_service_ == impl_->io_services_.size() )
			impl_->next_io_service_ = 0;

		return io_service;
	}
}