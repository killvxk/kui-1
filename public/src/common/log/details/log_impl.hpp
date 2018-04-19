#ifndef __LOG_LOG_IMPL_HPP
#define __LOG_LOG_IMPL_HPP

#include <cstdint>
#include <sstream>

namespace logger {

	enum class level_t
		: std::uint8_t
	{
		trace,
		debug,
		info,
		notice,
		warn,
		err,
		critical,
	};


	inline const wchar_t* to_wstr(level_t l)
	{
		static const wchar_t* level_names[]
		{
			L"trace", L"debug", L"info", L"notice", L"warning", L"error", L"critical",
		};

		return level_names[(std::uint8_t)l];
	}

	inline const char* to_str(level_t l)
	{
		static const char* level_names[]
		{
			"trace", "debug", "info", "notice", "warning", "error", "critical",
		};

		return level_names[(std::uint8_t)l];
	}


	namespace details {

		template < typename T >
		struct to_string_t;

		template <>
		struct to_string_t<char>
		{
			static const char* to(level_t l)
			{
				return to_str(l);
			}

			static char format()
			{
				return '%';
			}
		};

		template <>
		struct to_string_t<wchar_t>
		{
			static const wchar_t* to(level_t l)
			{
				return to_wstr(l);
			}

			static wchar_t format()
			{
				return L'%';
			}
		};


		template < typename CharT >
		void safe_printf(std::basic_ostringstream<CharT> &os, const CharT *s)
		{
			while( *s )
			{
				if( *s == to_string_t<CharT>::format() )
				{
					if( *(s + 1) == to_string_t<CharT>::format() )
					{
						++s;
					}
					else
					{
						throw std::runtime_error("invalid format string: missing arguments");
					}
				}
				os << *s++;
			}
		}

		template < typename CharT, typename T, typename... Args >
		void safe_printf(std::basic_ostringstream<CharT> &os, const CharT *s, const T &value, const Args &... args)
		{
			while( *s )
			{
				if( *s == to_string_t<CharT>::format() )
				{
					if( *(s + 1) == to_string_t<CharT>::format() )
					{
						++s;
					}
					else
					{
						os << value;
						s += 2;
						safe_printf(os, s, args...); // call even when *s == 0 to detect extra arguments
						return;
					}
				}
				os << *s++;
			}
		}
	}

	template < typename HandlerT, typename CharT, typename T, typename... Args >
	void log(const HandlerT &handler, level_t level, const CharT *s, const T &value, const Args &...args)
	{
		std::basic_ostringstream<CharT> os;
		details::safe_printf(os, s, value, args...);

		handler(level, os.str());
	}
}


#endif