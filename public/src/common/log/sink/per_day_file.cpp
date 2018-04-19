#include "per_day_file.hpp"


#include <iomanip>
#include <memory>

#include <system/system.hpp>
#include <task/timing_task.hpp>

#include <atlbase.h>
#include <atlconv.h>
#include <atltime.h>

#include "../log.hpp"
#include "sys_file.hpp"


namespace logger { namespace sink { 

	struct per_day_file_t::impl
	{
		log_t &log_; 
		task::timing_task_t &timing_task_;
		std::shared_ptr<file_t> sink_;

		impl(log_t &log, task::timing_task_t &timing_task, const std::string &name)
			: log_(log)
			, timing_task_(timing_task)
		{
			auto log_handler = [this, name]() 
			{
				sink_ = make_log_file(name);
				auto log_id = log_.add_sink(
					[this](const std::string &content)
				{
					sink_->write(content);
				});

				return log_id;
			};
			auto log_id = log_handler();

			CTime val = CTime::GetCurrentTime() + CTimeSpan(1, 0, 0, 0);
			CTime next_day{ val.GetYear(), val.GetMonth(), val.GetDay(), 0, 0, 0 };

			auto point = std::chrono::system_clock::from_time_t(next_day.GetTime());
			timing_task_.add(point, std::chrono::hours{ 24 }, [this, log_id, log_handler]() mutable
			{
				log_.erase_sink(log_id);
				log_id = log_handler();
			});

			timing_task_.add(std::chrono::system_clock::now(), std::chrono::seconds{ 10 }, [this]() 
			{
				sink_->flush();
			});
		}

		std::shared_ptr<file_t> make_log_file(const std::string &name)
		{
			std::stringstream os;
			auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

			std::tm tm = { 0 };
			localtime_s(&tm, &t);
			os << "log\\" << name << '-' << std::put_time(&tm, "%Y%m%d") << ".log";

			std::string path = (LPCSTR)CW2A(sys::app_path().c_str()) + os.str();
			return std::make_shared<file_t>(fs::path{ path });
		}
	};

	per_day_file_t::per_day_file_t(logger::log_t &log, task::timing_task_t &timing_task, const std::string &name)
		: impl_(std::make_unique<impl>(log, timing_task, name))
	{
		
	}

	per_day_file_t::~per_day_file_t()
	{

	}

	void per_day_file_t::write(const std::string &content)
	{
		impl_->sink_->write(content);
	}

	void per_day_file_t::flush()
	{
		impl_->sink_->flush();
	}

} }