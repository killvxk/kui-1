#pragma once

#include <Windows.h>

#include <network\protocol.hpp>

// TODO: reference additional headers your program requires here
#define FRAME_START_FLAG				'.mgs'

#define FRAME_CMD_QUERY					0x01
#define FRAME_CMD_READ					0x02
#define FRAME_CMD_INFO					0x04

#define CONNECT_TYPE_USERMAIN			2		// 虚拟盘主连接 (虚拟磁盘使用)

typedef struct VDiskRequest
{
	ULONG			flag;		// FRAME_START_FLAG
	ULONG			length;		// total length.
	ULONG			command;	// 命令号
	ULONG			status;		// 状态

	union
	{
		struct
		{
			ULONG			resv1[8];
		}Param;
		struct
		{
			LARGE_INTEGER	start;
			ULONG			number;
		}Read;
		struct
		{
			LARGE_INTEGER	start;
			ULONG			number;
		}Write;
		struct
		{
			UCHAR			type;		// 连接类型
			DWORD			rspid;		// 还原点ID
			DWORD			remoteip;	// 客户机IP
		}ConnectInfo;
	};


}VDISKREQUEST, *LPVDISKREQUEST;

typedef struct VDiskAck
{
	ULONG			flag;			// FRAME_START_FLAG
	ULONG			length;			// total length.
	ULONG			command;		// 命令号
	ULONG			status;			// 状态
}VDISKACK, *LPVDISKACK;


namespace svr {

	struct request_protocol_t
		: network::msg_in_t<request_protocol_t>
	{
		using request_t = VDISKREQUEST;

		request_t header_;

		request_protocol_t(const network::buffer_t &buffer)
			: network::msg_in_t<request_protocol_t>(buffer)
		{

		}

		template < typename StreamT >
		void pop_header(StreamT &os)
		{
			os.pop_pointer((char *) &header_, sizeof(header_));
		}
	};

	struct ack_protocol_t
		: network::msg_out_t<ack_protocol_t>
	{
		using ack_t = VDISKACK;

		ack_protocol_t(network::buffer_t &buffer, ULONG cmd)
			: network::msg_out_t<ack_protocol_t>(buffer, ack_t{ FRAME_START_FLAG, 0, cmd, 0 })
		{}

		template < typename StreamT >
		void push_header(StreamT &os, const ack_t &header)
		{
			os.push_pointer((char *) &header, sizeof(header));
		}

		template < typename StreamT >
		void buffer_complete(StreamT &os, network::buffer_t &buffer)
		{
			auto ack = reinterpret_cast<ack_t *>(buffer->data());
			ack->length = os.in_length();
			buffer->resize(ack->length);
		}
	};
}

namespace cli {

	inline VDISKREQUEST request_cmd(ULONG cmd, std::uint64_t offset, std::uint32_t size)
	{
		VDISKREQUEST req = {FRAME_START_FLAG, 0, cmd, 0};
		req.Write.start.QuadPart = offset;
		req.Write.number = size;

		return req;
	}

	struct request_protocol_t
		: network::msg_out_t<request_protocol_t>
	{
		using request_t = VDISKREQUEST;

		request_protocol_t(network::buffer_t &buffer, const request_t &req)
			: network::msg_out_t<request_protocol_t>(buffer, req)
		{

		}

		template < typename StreamT >
		void push_header(StreamT &os, const request_t &header)
		{
			os << header;
		}

		template < typename StreamT >
		void buffer_complete(StreamT &os, network::buffer_t &buffer)
		{
			auto ack = reinterpret_cast<request_t *>(buffer->data());
			ack->length = os.in_length();
			buffer->resize(ack->length);
		}
	};

	struct ack_protocol_t
		: network::msg_in_t<ack_protocol_t>
	{
		using ack_t = VDISKACK;
		ack_t header_;

		ack_protocol_t(const network::buffer_t &buffer)
			: network::msg_in_t<ack_protocol_t>(buffer)
		{}

		template < typename StreamT >
		void pop_header(StreamT &os)
		{
			os >> header_;
		}
	};
}


