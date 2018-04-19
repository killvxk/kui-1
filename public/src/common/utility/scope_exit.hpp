#ifndef __UTILITY_SCOPE_EXIT_HPP
#define __UTILITY_SCOPE_EXIT_HPP

#include <functional>
#include <memory>


namespace utility
{
	/** @
	* 利用RAII机制，退出作用域调用指定过程
	*/

	struct deletor_t
	{
		template < typename T >
		void operator()(T *p)
		{
			(*p)();
		}
	};
	
	template < typename D >
	auto make_scope_exit(D &&d)->std::unique_ptr<D, deletor_t>
	{
		return std::unique_ptr<D, deletor_t>(&d, deletor_t());
	}
}




#endif