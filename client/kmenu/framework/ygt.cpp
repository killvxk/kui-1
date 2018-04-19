#include "ygt.hpp"

#include <Windows.h>
#include <fstream>
#include <Iphlpapi.h>
#include <Winsock.h>
#include <vector>
#include <filesystem>


#include "aes.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")


	struct ygt_t::impl
	{
		std::string key_;
		std::string full_path_;
		impl()
		{
			const std::string kYGTFile = "yokagamesygt.tmp";
			const std::string kYGTKey = "t8tHuhCWW8gUN6bn";

			key_ = kYGTKey + get_mac_adress();
			full_path_ = ygt_path() + kYGTFile;
		}

		std::string get_mac_adress()
		{
			std::string strMac;
			DWORD dwSize = 0;
			PIP_ADAPTER_INFO pAdapterInfo = NULL;
			if (GetAdaptersInfo(pAdapterInfo, &dwSize) != ERROR_BUFFER_OVERFLOW)
				return "";
			std::vector<char> buf(dwSize);
			pAdapterInfo = (PIP_ADAPTER_INFO)(buf.data());
			if (GetAdaptersInfo(pAdapterInfo, &dwSize) != NOERROR)
			{
				return "";
			}
			PIP_ADAPTER_INFO pAdapter = pAdapterInfo, pNextAdapter = pAdapterInfo;
			while (pNextAdapter)
			{
				pAdapter = pNextAdapter;
				pNextAdapter = pAdapter->Next;

				MIB_IFROW IfRow = { 0 };
				IfRow.dwIndex = pAdapter->Index;
				if (GetIfEntry(&IfRow) != NO_ERROR || inet_addr(pAdapter->IpAddressList.IpAddress.String) == 0 ||
					inet_addr(pAdapter->GatewayList.IpAddress.String) == 0 ||
					IfRow.dwOperStatus != IF_OPER_STATUS_OPERATIONAL)
					continue;

				char szMac[MAX_PATH] = { 0 };
				sprintf_s(szMac, _countof(szMac), "%02X%02X%02X%02X%02X%02X%02X%02X",
					pAdapter->Address[0],
					pAdapter->Address[1],
					pAdapter->Address[2],
					pAdapter->Address[3],
					pAdapter->Address[4],
					pAdapter->Address[5],
					pAdapter->Address[6],
					pAdapter->Address[7]);

				strMac = szMac;
				break;
			}
			return strMac;
		}

		std::string ygt_path()
		{
			char path[MAX_PATH] = { 0 };
			::GetTempPathA(MAX_PATH, path);
			return std::string{ path };
		}

		bool write( const std::string & content )
		{
			if (content.empty())
				return false;

			AES aes((unsigned char*)key_.c_str());
			auto length = content.length() + (16 - (content.length() % 16));

			void *rdata = aes.Cipher((unsigned char*)content.data(), length);
		
			std::ofstream outfile(full_path_, std::ofstream::binary);
			if (!outfile)
				return false;

			outfile.write(reinterpret_cast<const char*>(rdata), length);

			return true;
		}

		std::string read( )
		{
			std::ifstream infile(full_path_, std::ifstream::binary);
			if (!infile)
				return std::string{};

			infile.seekg(0, std::ios::end);
			auto length = infile.tellg();
			infile.seekg(0, std::ios::beg);

			std::vector<char> buf((std::size_t)length);
			infile.read(buf.data(), buf.size());

			AES aes((unsigned char*)key_.c_str());
			aes.InvCipher((void*)buf.data(), (std::size_t)length);

			return{ buf.data() };
		}

		bool del()
		{
			return true;
		}
	};
	
	ygt_t::ygt_t()
		:impl_(std::make_unique<impl>())
	{}

	ygt_t::~ygt_t()
	{}

	bool ygt_t::write(const std::string & content )
	{
		return impl_->write(content);
	}

	std::string ygt_t::read()
	{
		return impl_->read();
	}

	bool ygt_t::del()
	{
		return impl_->del();
	}
