#include "vdisk_msg.hpp"
#include "vdisk_protocol.hpp"

#include <network/server/server.hpp>

static const std::uint32_t SECTOR_SIZE = 512;


network::buffer_t handle_query(network::buffer_t &buffer, const VDISKREQUEST &req)
{
	svr::ack_protocol_t ack(buffer, req.command);
	ack << (100 * 1024ull * 1024 * 1024) / SECTOR_SIZE;

	return ack.buffer();
}

network::buffer_t handle_read(network::buffer_t &buffer, const VDISKREQUEST &req)
{
	svr::ack_protocol_t ack(buffer, req.command);
	
	static std::vector<char> data;
	data.resize(req.Read.number * SECTOR_SIZE, 0X01);

	ack << data;

	return ack.buffer();
}

network::buffer_t handle_info(network::buffer_t &buffer, const VDISKREQUEST &req)
{
	svr::ack_protocol_t ack(buffer, req.command);

	return ack.buffer();
}



network::buffer_t handle_msg(const network::session_ptr &session, const network::buffer_t &recv_buffer)
{
	svr::request_protocol_t req(recv_buffer);
	auto ctx = network::session_context(session);
	//network::buffer_t &send_buffer = *reinterpret_cast<network::buffer_t *>(ctx);

	network::buffer_t send_buffer = network::make_buffer(0);
	switch( req.header_.command )
	{
	case FRAME_CMD_QUERY:
		send_buffer = handle_query(send_buffer, req.header_);
	break;

	case FRAME_CMD_READ:
		send_buffer = handle_read(send_buffer, req.header_);
	break;

	case FRAME_CMD_INFO:
		send_buffer = handle_info(send_buffer, req.header_);
	break;

	default:
		assert(0);
	break;
	}

	return send_buffer;
}