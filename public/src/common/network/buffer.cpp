#include "buffer.hpp"

namespace network {

	buffer_t make_buffer(std::uint32_t size)
	{
		auto buffer = std::make_shared<buffer_t::element_type>();
		buffer->resize(size);

		return buffer;
	}
}