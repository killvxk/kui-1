#ifndef __TASK_TIMING_TASK_HPP
#define __TASK_TIMING_TASK_HPP

#include <memory>
#include <chrono>
#include <functional>


namespace task {

	struct timer_handle_t;
	using timer_ptr = std::shared_ptr<timer_handle_t>;

	struct timing_task_t
	{
		struct impl;
		std::unique_ptr<impl> impl_;

		timing_task_t();
		~timing_task_t();

		bool start();
		void stop();

		timer_ptr add(std::chrono::system_clock::time_point, 
			std::chrono::system_clock::duration, 
			const std::function<void()> &);
		void erase(const timer_ptr &);
	};
}


#endif