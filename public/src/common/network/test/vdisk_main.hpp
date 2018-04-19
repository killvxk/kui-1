#pragma once

#include <iostream>


#include <network\server\server.hpp>
#include <network\client\client.hpp>

#include "vdisk_protocol.hpp"
#include "vdisk_msg.hpp"


void vdisk_svr_test()
{
	// bind error handler/msg size handler
	network::server_t svr([](const std::error_code &err)
	{
		std::cerr << err.value() << ' ' << err.message() << std::endl;
	}, []()
	{
		return (std::uint32_t)sizeof(VDISKREQUEST);
	}, [](const char *header)
	{
		return reinterpret_cast<const VDISKREQUEST *>(header)->length;
	});

	// bind session message handler
	svr.reg_msg_handler([](const network::session_ptr &session, const network::buffer_t &recv_msg)
	{
		auto buffer = handle_msg(session, recv_msg);

		if( buffer )
			network::send_msg(session, buffer);
	});

	// bind session connected/disconnected handler
	svr.reg_status_handler(
		[](const network::session_ptr &session)
	{
		std::cout << "session enter: " << network::session_remote_ip(session) << " "
			<< network::session_local_ip(session) 
			<< std::endl;

		auto buffer = new network::buffer_t(network::make_buffer(0));
		network::session_context(session, buffer);
	}, 
		[](const network::session_ptr &session)
	{
		std::cout << "session leave: " << network::session_remote_ip(session) << std::endl;

		auto buffer = network::session_context(session);
		delete buffer;
	});

	// choose io service
	std::uint32_t thr_num = std::thread::hardware_concurrency();
	svr.start(5050, thr_num);
	
	std::cin.get();
	svr.stop();
}


void vdisk_response_cli_test()
{
	network::response_client_t cli([](const std::error_code &err)
	{
		std::cerr << err.message() << std::endl;
	}, []()
	{
		return (std::uint32_t)sizeof(VDISKACK);
	}, [](const char *data)
	{
		return reinterpret_cast<const VDISKACK *>(data)->length;
	});

	/*cli.set_heartbeat(std::chrono::seconds{5}, []() 
	{
		std::cerr << "heartbeat" << std::endl;
		cli::request_protocol_t req(network::make_buffer(0), cli::request_cmd(FRAME_CMD_QUERY, 0, 0));
		return req.buffer();
	});*/

	auto ret = cli.start("127.0.0.1", 5050, std::chrono::seconds(5));
	if( !ret )
	{
		std::cerr << "connect error" << std::endl;
		return;
	}

	{
		cli::request_protocol_t req(network::make_buffer(0), cli::request_cmd(FRAME_CMD_QUERY, 0, 0));

		auto recv_buffer = cli.send(req.buffer(), std::chrono::seconds{3});
		if( recv_buffer )
		{
			cli::ack_protocol_t ack(recv_buffer);
		}
	}
	
	{
		cli::request_protocol_t req(network::make_buffer(0), cli::request_cmd(FRAME_CMD_QUERY, 0, 0));

		auto recv_buffer = cli.send(req.buffer(), std::chrono::seconds{3});
		if( recv_buffer )
		{
			cli::ack_protocol_t ack(recv_buffer);
		}
	}
	

	std::cin.get();

	cli.stop();
}


void vdisk_normal_cli_test()
{
	network::normal_client_t cli([](const std::error_code &err)
	{
		std::cerr << err.message() << std::endl;
	}, []()
	{
		return (std::uint32_t)sizeof(VDISKACK);
	}, [](const char *data)
	{
		return reinterpret_cast<const VDISKACK *>(data)->length;
	});

	auto ret = cli.start("127.0.0.1", 5050, [](const network::buffer_t &recv_msg)
	{
		if( !recv_msg )
			return;

		cli::ack_protocol_t ack(recv_msg);
	}, std::chrono::seconds(5));
	if( !ret )
	{
		std::cerr << "connect error" << std::endl;
		return;
	}


	{
		cli::request_protocol_t req(network::make_buffer(0), cli::request_cmd(FRAME_CMD_QUERY, 0, 0));

		cli.async_send(req.buffer());
	}
	
	{
		cli::request_protocol_t req(network::make_buffer(0), cli::request_cmd(FRAME_CMD_QUERY, 0, 0));

		cli.async_send(req.buffer());
	}

	std::cin.get();

	cli.stop();
}