#ifndef __NETWORK_PROTOCOL_HPP
#define __NETWORK_PROTOCOL_HPP

#include <serialize/serialize.hpp>
#include "config.hpp"


namespace network {


	using serialize_in_t = serialize::i_serialize;
	using serialize_out_t = serialize::o_dynamic_serialize;

	
	template < typename DerivedT >
	struct msg_in_t
	{
		buffer_t buffer_;
		serialize_in_t in_;

		explicit msg_in_t::msg_in_t(const buffer_t &buffer)
			: buffer_(buffer)
			, in_(buffer_->data(), (std::uint32_t)buffer_->size())
		{
			static_cast<DerivedT *>(this)->pop_header(in_);
		}

		msg_in_t(const msg_in_t &) = delete;
		msg_in_t &operator=(const msg_in_t &) = delete;


		template < typename T >
		msg_in_t &operator>>(T &val)
		{
			in_ >> val;
			return *this;
		}
	};


	
	template < typename DerivedT >
	struct msg_out_t
	{
		buffer_t buffer_;
		serialize_out_t out_;

		template < typename AckT >
		explicit msg_out_t(buffer_t &buffer, const AckT &header)
			: buffer_(buffer)
			, out_(*buffer_)
		{
			static_cast<DerivedT *>(this)->push_header(out_, header);
		}

		msg_out_t(const msg_out_t &) = delete;
		msg_out_t &operator=(const msg_out_t &) = delete;

		const buffer_t &msg_out_t::buffer()
		{
			static_cast<DerivedT *>(this)->buffer_complete(out_, buffer_);

			return buffer_;
		}

		template < typename T >
		msg_out_t &operator<<(const T &val)
		{
			out_ << val;
			return *this;
		}
	};
}	

#endif
